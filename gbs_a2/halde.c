#include "halde.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

int MBLOCKSIZE = sizeof(struct mblock);

/// Heap-memory area.
char memory[SIZE];

/// Pointer to the first element of the free-memory list.
static struct mblock *head = NULL;

/// Helper function to visualise the current state of the free-memory list.
void halde_print(void) {
	struct mblock* lauf = head;

	if (head == MAGIC) {
		printf("NO MEMORY AVAILABLE\n");
		return;
	}

	// Empty list
	if ( head == NULL) {
		fprintf(stderr, "(empty)\n");
		return;
	}

	// Print each element in the list
    fprintf(stderr, "HEAD:  ");
	while ( lauf ) {
		fprintf(stderr, "(addr: 0x%08zx, off: %7zu, ", (uintptr_t) lauf, (uintptr_t)lauf - (uintptr_t)memory);
		fflush(stderr);
		fprintf(stderr, "size: %7zu)", lauf->size);
		fflush(stderr);

		if ( lauf->next != NULL ) {
			fprintf(stderr, "\n  -->  ");
			fflush(stderr);
		}
		lauf = lauf->next;
	}
	fprintf(stderr, "\n");
	fflush(stderr);
}

void initlist() {
			// initiliaze list
			head = (struct mblock*) memory;
			head->size = SIZE - MBLOCKSIZE;
			head->next = NULL;
}

// Return the first mblock with a size greater or equal the size
struct mblock *get_firstfit(size_t size) {
	// If the head points to magic, it means our whole list is full, thus there is no free memory available
	if(head == MAGIC)
		return MAGIC;

	// The head is not initialized.
	if(head == NULL) {
		initlist(); // Initiliaze it
	}

	// Checking if the head has enough room
	if(head->size >= size)
		return head;  // Yes, return the head


	// Search for a suiting mblock
	struct mblock* prev = head;
	for(struct mblock *node = head; node != NULL; node = node->next) {
		if(node->size >= size) { // Mblock has enough memory
			return prev; // Return mblock
		}
		prev = node;
	}

	// If there is no block available with the given size, return MAGIC
	return MAGIC;
}

// Checks if mblock has enough room for another mblock, if so, we divide and link the new mblock into the list
void divide_if_dividible(struct mblock *prev, size_t size) {
	// We got no valid mblock. So we terminate
	if(prev == MAGIC || prev == NULL)
		return;

	// We got the head pointer
	if(prev == head) {
		// Checking if there is enough room for a second mblock
		if(prev->size >= MBLOCKSIZE + size + 1) {
			// There is indeed enough room
			// We calculate the size of the new mblock
			int newsize = prev->size - MBLOCKSIZE - size;
			// We save the next mblock in line
			struct mblock *p = prev->next;

			// We create a new mblock at the end of the current mblock, which is getting reserved for use
			prev->next = (struct mblock*) (prev->memory + size);
			// Helper variable
			struct mblock *pn = prev->next;

			// We update our reserved mblock
			prev->next->size = newsize; // Set the available size (after splitting) for our new block
			prev->next->next = p; // Point to the next mblock in line

			// Mark the reserved block as in use and update its size
			head->size = size;
			head->next = MAGIC;

			// Point the head to our new created mblock
			head = pn;

			// Splitted, reserved and done
			return;
		} else { // There is not enough room for another mblock, so we use the whole available size (it may be == size or a bit more (<=16bit))
			if(head->next == NULL) { // Checks if it is the last available mblock
				head->next = MAGIC; // Marks the mblock as used
				head = MAGIC; // Marks the list as FULL
			} else { // There is at least one more block available.
				struct mblock *p = head->next; // Saves the adress of the next mblock in line
				head->next = MAGIC; // Set mblock as used
				head = p; // Points the head to the adjacent mblock
			}
			// We are done for the case prev == head
			return;
		}
	}

	// Since we've got a mblock which is != head, we have about the same procedure
	// First we save the next block in line
	struct mblock *node = prev->next;

	// Checks if there is enough room for another mblock
	if(node->size >= MBLOCKSIZE + size + 1) {
		int oldsize = node->size; // Save size
		struct mblock *p = node->next; // save next mblock in line

		prev->next = (struct mblock*) (node->memory + size); // link new mblock
		prev->next->size = oldsize - size - MBLOCKSIZE; // set new mblock size
		prev->next->next = p; // link node to next node in line

		node->next = MAGIC; // Mark the mblock as used
	} else { // Block is not big enough for splitting
		prev->next = prev->next->next; // Skip the current mblock in line
		node->next = MAGIC; // Mark the mblock as used
	}

	return; // We are done!
}


// Suche den ersten Speicherbereicher, der für die angeforderte Speichermenge groß genug ist und entferne ihn aus der Freispeicherliste
void *halde_malloc (size_t size) {
	// Error! If the size if zero, we don't need any memory. So we return nothing
	if(size == 0)
		return NULL;
	// If the head is NULL, we have no list. So we iniate our list
	// Doppelt, passiert bereits in get_firstfit.
	if(head == NULL)
		initlist(); // Function to iniate our list

	// We get the first mblock with the right size
	struct mblock *node = get_firstfit(size);

	// Ist der Speicherbereich größer als benötigt und verbleibt genügend Rest, so wird dieser Speicherbereich geteilt und der Rest wird mit Hilfe eines neuen Listenelements in
	// die Freispeicherliste eingehängt
	divide_if_dividible(node, size);

	// Der zurückgelieferte Zeiger zeigt auf den char Speicherbereich, welcher hinter dem struct head liegt
	if(node != NULL && node != MAGIC) { // Wir überprüfen ob genügend Speicher vorhanden war
		return (char*) node->memory;
	}
	else {
		// We didn't find a mblock with enough capacity! We throw an "Not enough memory" error and return nothing!
		errno = ENOMEM;
		return NULL;
	}
}

// Inserts the given memory block into the memory list at the right position
void insert_back(struct mblock *node) {

	// If the head is MAGIC we don't have free memory blocks. So the given block is the new head of the list!
	if(head == MAGIC) {
		// Points the head at our node
		head = node;
		// We don't have any adjacent mblocks
		head->next = NULL;
		// Done and return
		return;
	}

	// Checks if the adress of the node is ahead of our head pointer
	if(node < head) {
		// It is indeed ahead!
		// Our given block points sets the (current) head as the next block in line
		node->next = head;
		// Update our head pointer to our block
		head = node;
		// Done
		return;
	}

	// A helper variable for our for upcoming for loop
	struct mblock *prev;

	// Iterate through the memory block list to find the right position
	for(struct mblock *curr = head; curr != NULL; curr = curr->next) {
		// Checks if our mblock is ahead of the current mblock
		if(node < curr) {
			// It is ahead! We found the right position for our mblock
			// The previous mblock needs to point to our given mblock, since the current mblock comes after our mblock
			prev->next = node;
			// Since our mblock comes is ahead of the current mblock, we set the current mblock as the next in line mblock
			node->next = curr;
			//Done
			return;
		}
			// Saves the current mblock, so we can always access the previous mblock more easily
			prev = curr;
	}
	// We have not found our right position, that means, our block will be the last block in line. Since we saved the last mblock in our variable prev we can update the prev->next pointer and let it point to our mblock
	prev->next = node;
	// Our mblock is the last one in line. Therefore we have no follower
	node->next = NULL;
	// Done! :)
	return;
}


// Checks if our list has no direct adjacent memory blocks. Returns 1 for true and 0 for false
int is_merged() {
	// Iterates through the list to find direct adjacent memory blocks
	for(struct mblock *node = head; node != NULL; node = node->next) {
		// Checks if the blocks are directly adjacent
		if((node->memory + node->size) == (char*) node->next) {
			// It's adjacent! The list is not completely merged!
			return 0;
		}
	}
	// Great, the list is completely merged and has no direct adjacent neighbors
	return 1;
}



void merge_free() {
	// Iterate through our memory list to find adjacent blocks
	for(struct mblock *node = head; node != NULL; node = node->next) {
		// Is the area of memory ending at the start of our next mblock?
		if((node->memory + node->size) == (char*)node->next){
			// We've found an adjacent mblock!

			// Add the adjacent block size + the size of our structure to our current block
			node->size = node->size + node->next->size + 16;
			// Update the next point of the current block to the next mblock after our merged neighbor
			node->next = node->next->next;

			// Check if all blocks are merged
			if(is_merged())
				return; // They are all merged
			else
				merge_free(); // Merge again
		}
	}
}

void halde_free (void *ptr) {
	// If ptr is NULL or MAGIC there is nothing to be freed
	if(ptr == NULL || ptr == MAGIC)
		return;

	// Create a temporary mblock pointer to operate more easily
	struct mblock *node = (struct mblock*) ((char*) ptr - 16);

	// If the node->next isn't pointing to MAGIC we've got a wrong mblock, so we abort
	if(node->next != MAGIC)
		abort();

	// We insert our memory block back into our memory block list
	insert_back(node);

	// We merge adjacent memory blocks
	merge_free();

	return;
}
