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
#include <pthread.h>
#include <semaphore.h>
#include "triangle.h"

// TODO: shared state variables
char args[256];
int numThreads;
sem_t mutex, semRead, semWrite;
int updateFlag = 0;
int succ;
volatile int i = 0; //Anzahl aktiven Threads
volatile int numBoundary = 0;  //schlechte lösung maybe geht nur für 1 Thread?
volatile int numInterior = 0;  //schlechte lösung maybe geht nur für 1 Thread?
volatile int finished = 0;
// TODO: helper functions
void threadResult(int boundary, int interior) { //schlechte lösung maybe geht nur für 1 Thread?
  numBoundary = numBoundary + boundary;
  numInterior = numInterior + interior;
}


struct triangle initTriangle(struct coordinate p1, struct coordinate p2, struct coordinate p3) {
  struct triangle dreieck = {{p1,p2,p3}};
  return dreieck;
}

struct coordinate initCoord(int x, int y) {
  struct coordinate coor = {x, y};
  return coor;
}

void * nebenThread(void *arg) {
  struct triangle *dreieck = (struct triangle*)arg;
  sem_wait(&semRead);
  sem_wait(&mutex);
  i++;
  countPoints(dreieck, threadResult);
  i--;
  finished++;
  updateFlag = 1;
  sem_post(&mutex);
  sem_post(&semWrite);
  return NULL;
}

void * ausgabeThread(void *arg) {
  sem_wait(&semWrite);
  sem_wait(&mutex);
  if (updateFlag == 1) {
    printf("Found %d boundary and %d interior points, %d active threads, %d finished threads\n", numBoundary, numInterior, i, finished);
    updateFlag = 0;
  }
  sem_post(&mutex);
  sem_post(&semRead);
  return NULL;
}

int main(int argc, char * argv[]) {
  // TODO: implement me
  if (argc != 2) return 0; // muss ein arg gibt bei kommandozeile!
  char * anzahl = argv[1];
  numThreads = atoi(anzahl);

  if (sem_init(&mutex, 0, 1)) {
    exit(EXIT_FAILURE);
  }
  if (sem_init(&semRead, 0, numThreads)) {
    exit(EXIT_FAILURE);
  }
  if (sem_init(&semWrite, 0, 0)) {
    exit(EXIT_FAILURE);
  }

  pthread_t threadsList[2048];
  pthread_t ausThread;
  int x1,y1,x2,y2,x3,y3;

  while(1) {
    succ = pthread_create(&ausThread, NULL, ausgabeThread, NULL);
    fflush(stdout);
    if (fgets(args, sizeof(args), stdin) == NULL) return 0;
    if (sscanf(args, "(%d,%d),(%d,%d),(%d,%d)", &x1,&y1,&x2,&y2,&x3,&y3) != 6) {
      printf("Format: (x1,y1),(x2,y2),(x3,y3)\n");
      continue;
    }
    int len = strlen(args);
    if (*(args+len-2) != ')') {
      printf("Format: (x1,y1),(x2,y2),(x3,y3)\n");
      continue;
    }
    struct coordinate p1 = initCoord(x1, y1);
    struct coordinate p2 = initCoord(x2, y2);
    struct coordinate p3 = initCoord(x3, y3);
    struct triangle dreieck = initTriangle(p1, p2, p3);
    if (succ) exit(EXIT_FAILURE);
    succ = pthread_create(&threadsList[i], NULL, nebenThread, &dreieck);
    if (succ) exit(EXIT_FAILURE);
    pthread_detach(threadsList[i]);
  }

  // ACTIVATE _MEDIUM_TESTCASES_NOT_IMPLEMENTED_MARKER: remove this line to enable medium size testdata
  // ACTIVATE _LARGE_TESTCASES_NOT_IMPLEMENTED_MARKER: remove this line to enable large size testdata
  // ACTIVATE _DYNAMIC_TESTCASES_NOT_IMPLEMENTED_MARKER: remove this line to enable dynamically generated testdata
  // MULTITHREADING _NOT_IMPLEMENTED_MARKER: remove this line to activate testcases which expect multiple worker threads
  return 1;
}
