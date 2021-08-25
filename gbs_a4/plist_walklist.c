#include <stdlib.h>
#include "plist.h"


void walkList(list *list, int (*callback) (pid_t, const char *) ) {
	// TODO: implement me, Subtask d
  struct qel *curr = list->head;

  while (curr) {
    // Do something
    (*callback)(curr->pid, curr->cmdLine);
    curr = curr->next;
  }
}
