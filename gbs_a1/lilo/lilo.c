#include <stdio.h>
#include <stdlib.h>

typedef struct list_element {
	//TODO: implement fields
	int data;
	struct list_element *next;
} list_element_t;

typedef struct list {
	// TODO: implement fields
	list_element_t *head;
} list_t;


int list_append(list_t *list, int value) {
	// TODO: implement body
	list_element_t *node = malloc(sizeof(list_element_t) * 1);
	// malloc success?
	if (node == NULL) {
		return -1;
	}
	// value must be positive
	if (value < 0) {
		return -1;
	}

	node->data = value;
	node->next = NULL;

	// if list empty add node
	if (list->head == NULL) {
		list->head = node;
		return value;
	}

	list_element_t *prev = NULL;
	list_element_t *cur = list->head;

	// goto end of list, check if value in list vorhanden ist
	while (cur != NULL) {
		if (cur->data == value) {
			return -1;
		}
		prev = cur;
		cur = cur->next;
	}

	// end of list reached. prev = end element
	prev->next = node;
	return value;
}

int list_pop(list_t *list) {
	// TODO: implement body
	int pop_value;
	// list empty dann error
	if (list->head == NULL) {	return -1;}

	// pop oldest data
	list_element_t *popped = list->head;
	list_element_t *cur = list->head->next;
	list->head = cur;
	pop_value = popped->data;
	free(popped);
	return pop_value;
}

int main (int argc, char* argv[]) {
	list_t list = {0}; //TODO: initialize

	printf("insert 47: %d\n", list_append(&list, 47));
	printf("insert 11: %d\n", list_append(&list, 11));
	printf("insert 23: %d\n", list_append(&list, 23));
	printf("insert 11: %d\n", list_append(&list, 11));

	printf("remove: %d\n", list_pop(&list));
	printf("remove: %d\n", list_pop(&list));
	exit(EXIT_SUCCESS);
}
