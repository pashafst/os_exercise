// TODO: includes
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#include "plist.h"

#define TRUE 1

char buf[2048] = {0};
char* token;
char* argv[2048]; // {echo, test, ls}
int count = 0;
char commandLine[2048];
list childList = {0};
bool bg;
bool lineEmpty;
char cmdBuffer[2048];

int showJobs(pid_t pid, const char * cmdLine) {
  printf("[%d] %s\n", pid, cmdLine);
  return 1;
}

// Only for debugging
void printChildList(list * list) {
  struct qel *curr = list->head;
  while (curr) {
    printf("[%d] %s\n", curr->pid, curr->cmdLine);
    curr = curr->next;
  }
}

void clearARGV() {
  for(int i = 0; i < count; i++) {
    argv[i] = NULL;
  }
  count = 0;
}

int isBackgroundProcess(char *str) { // cd .. & process -a -b arg1 &
  char *c = str;

  while(*c != '\0') {
    if(*c == '&' && *(c+1) == '\0') {
      *c = '\0';
      return 1;
    }
    c++;
  }
  return 0;
}

// TODO: helpers (forward decls) for command parsing, job printing, book keeping
void tokenizeInput(char* str) {
    clearARGV();
    token = strtok(str, " "); // Initiliaze strtok and get first token (program name)
    while (token != NULL) { // Parse every token
        argv[count] = token; // Save token on pos count
        token = strtok(NULL, " "); // Get next token
        count++; // Inkrementiere Position
    }
    if (count == 0) return;
    if( isBackgroundProcess(argv[count-1]) ) {
      argv[count] = "&";
    } else {
      count--;
    }
}

int handleIntern() {
  if(strcmp("cd", argv[0]) == 0) {
    if (argv[2]) {
      printf("too many args.\n");
      return 1;
    }

    if(chdir(argv[1]) == -1)
      printf("Can't access directory %s\n", argv[1]);
    return 1;
  }
  if(strcmp("exit", argv[0]) == 0) {
    exit(EXIT_SUCCESS);
    return 1;
  }
  if(strcmp("jobs", argv[0]) == 0) { // Für aufgabe d, krieg noch keine Punkte, sollte eig schon funktionieren?
    walkList(&childList, showJobs);
    return 1;
  }
  return 0;
}

void getPromptSymbol() {
  char buffer[100] = {0};
  getcwd(buffer, 100);
  fprintf(stderr, "%s: ", buffer);
}

void checkFinishBG() {
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    removeElement(&childList, pid, cmdBuffer, 2048);
    fprintf(stderr, "Exitstatus [%s] = %d\n", cmdBuffer, WEXITSTATUS(status));
  }
}
int prompt() {
  lineEmpty = 0;
  checkFinishBG();
  getPromptSymbol(); // damit unser shell wie ein echtes profi shell aussieht
  int len;
  long max_line = sysconf(_SC_LINE_MAX); // Nimm MAX LINE von sys config

  if (fgets(buf, max_line+2, stdin) == NULL) {
    printf("\n");
    return 0;
  }
  len = strlen(buf);
  if (len == 1 && buf[0] == '\n') {
    lineEmpty = 1;
    return 1;
  }
  if(len == 1 && buf[0] != '\n') //
    return 0;
  if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0'; //ende des string ist \n, wir wollen das weg machen
  return 1;
}



int main () {
  while(prompt()) {
    if (strlen(buf) > sysconf(_SC_LINE_MAX)) continue;
    if (lineEmpty) continue;
    bg = 0;
    strcpy(commandLine, buf);
    tokenizeInput(buf); //cd . test
    if(handleIntern())
      continue;


    if(strcmp(argv[count], "&") == 0) {
      bg = 1;
      argv[count] = NULL;
    }


    // Prüfen ob internes Command (beispielsweise cd)
    pid_t pid;
    pid = fork(); // eltern bumsen krieg ein kind

    if (pid < 0) { // Wenn nicht geforked
      perror("ERROR");
    }

    else if (pid == 0) { // Kind Process
      // TODO
      if (execvp(argv[0], argv) == -1) perror("ERROR");
      exit(EXIT_FAILURE);
    }

    // Eltern
    int status;
    if (bg) {
      insertElement(&childList, pid, commandLine);
      continue; // Wenn Hintergrund dann wieder nach oben
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      fprintf(stderr, "Exitstatus [%s] = %d\n", commandLine, WEXITSTATUS(status));
    }
  }
  return 0;
	// TODO: implement me
	// PROMPT_NOT_IMPLEMENTED_ MARKER remove this line to enable prompt related testcases
	// CHILD_NOT_IMPLEMENTED_ MARKER remove this line to enable testaces which execute commands
	// STATUS_NOT_IMPLEMENTED_ MARKER remove this line to enable testcases for the status line after a child terminates
	// CD_NOT_IMPLEMENTED_ MARKER remove this line to enable cd related testcases
	// BACKGROUND_NOT_IMPLEMENTED_ MARKER remove this line to enable testcases for background tasks
	// JOBS_NOT_IMPLEMENTED_ MARKER remove this line to enable cd related testcases
}


// TODO: helper implementation
