// TODO: includes
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

// TODO: global variables
int curRound = 0;
int endRound = 0;
struct timeval fastest;
struct timeval sum;
struct timeval startTime;
struct timeval handler_value;
struct timeval current_value;
struct timeval rundeTime;

void printState() { // ToDo ausgabe auf stderr
  fprintf(stderr, "fastest: %01ld:%02ld.%04ld\n", fastest.tv_sec/60, fastest.tv_sec - (fastest.tv_sec/60) , fastest.tv_usec % 10000);
  fprintf(stderr, "sum: %01ld:%02ld.%04ld\n", sum.tv_sec/60, sum.tv_sec - (sum.tv_sec/60) , sum.tv_usec % 10000);
}
void endRace() { // ToDo ausgabe auf stderr
  fprintf(stderr, "race canceled\n");
  exit(0);
}

void printRound() { // ToDo ausgabe auf stderr
  fprintf(stderr, "lap %3d: %01ld:%02ld.%04ld\n", curRound, rundeTime.tv_sec/60, rundeTime.tv_sec - (rundeTime.tv_sec/60) , rundeTime.tv_usec % 10000); // Print rundeTime
}

// TODO: helpers, signal handlers
void handle_time() {
  if(curRound == 0) { // initialize
    fprintf(stderr, "race started, press Ctrl+C for next round!\n");
    curRound++;
    timerclear(&sum); // sekunde = 00000
    return;
  }



  timersub(&current_value, &startTime, &rundeTime);
  timeradd(&sum, &rundeTime, &sum);

  if (!timerisset(&fastest))
    fastest = rundeTime;

  if (timercmp(&fastest, &rundeTime, >) != 0) {
    fastest = rundeTime;
  }

  if(curRound == endRound + 1) { // print end state
    printRound();
    printState();
    endRace();
    return;
  }

  printRound();
  curRound++;

  // Print time difference
  // Get new timestamp // Start new round
  // Increase Curround
}



static void my_handler(int signum) {
  if(signum == SIGINT) {
    startTime = current_value;
    gettimeofday(&current_value, NULL);
    handle_time(curRound);
  }

  if(signum == SIGTERM) {
    endRace();
  }
}

int main(int argc, char *argv[]) {
  // Handle wrong arg
  if (argc != 2) {
    fprintf(stderr, "Usage: ./ticker <number of laps>\n");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa;
  sa.sa_handler = my_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if(sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
    fprintf(stderr, "Fehler bei der Registrierung des Signal Handlers!\n");
    abort();
  }

  char *c = argv[1];
  while(*c != '\0') {
    if(*c < '0' || *c > '9') {
      fprintf(stderr, "Die Anzahl der Runden ist eigentlich immer eine Zahl\n");
      exit(EXIT_FAILURE);
    }
    c++;
  }

  endRound = atoi(argv[1]);
  if(endRound <= 0) {
    fprintf(stderr, "Ich glaube du hast das Konzept der Runden nicht verstanden.\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "pid: %d\n", (int)getpid());
  fprintf(stderr, "Signal handler installed.\n");
  fprintf(stderr, "Ready...\n");

  for(int i = 0; i <= endRound; i++) {
    pause();
  }

  printState();

  return 0;
}
// TODO: implement main
// ARGUMENT _NOT_IMPLEMENTED_MARKER remove to activate argument parsing tests
// BASIC _NOT_IMPLEMENTED_MARKER remove to activate simple straight forward test runs without SIGTERM
// SIGTERM _NOT_IMPLEMENTED_MARKER remove to enable testaces which send SIGTERM to stop a race
