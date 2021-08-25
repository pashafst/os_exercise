// TODO: includes
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define READ 0
#define WRITE 1

// TODO: globals

// TODO: helpers


// seq 1 MAX | awk AWK_VORSCHRIFT | grep GREP_VORSCHRIFT > OUTPUT_FILE

int main(int argc, char *argv[]) {
  int pipe_A[2];
  int pipe_B[2];
  pid_t pid_A, pid_B, pid_C;

  pipe(pipe_A);

  if( !(pid_A = fork()) ) {
      close(pipe_A[0]); // A-read not needed here

      close(1);
      dup2(pipe_A[1], WRITE);
      close(pipe_A[1]); //do not pass A-write twice

      execlp("seq", "seq", "1", argv[1],  NULL);
  }

  close(pipe_A[1]); // A-write not needed anymore

  pipe(pipe_B);

  if( !(pid_B = fork()) ) {
      close(pipe_B[0]); // B-read not needed here

      close(0);
      dup2(pipe_A[0], READ);
      close(pipe_A[0]); //do not pass A-read twice


      close(1);
      dup2(pipe_B[1], WRITE);
      close(pipe_B[1]); //do not pass B-write twice

      execlp("awk", "awk", argv[2], NULL);
  }
  close(pipe_A[0]); // A-read not needed anymore
  close(pipe_B[1]); // B-write not needed anymore

  if( !(pid_C = fork()) ) {

      close(0);
      dup2(pipe_B[0], READ);
      close(pipe_B[0]); // do not pass B-read twice

			if(strcmp(argv[4], "-") != 0) {
				FILE *fp = fopen(argv[4], "w");
				chmod(argv[4], 0640);
				dup2(fileno(fp), 1);
				fclose(fp);
			}

      execlp("grep", "grep", argv[3], NULL);
  }
	wait(&pid_A);
	wait(&pid_B);
	wait(&pid_C);

	close(pipe_B[0]);
  close(pipe_B[1]); // B-read not needed anymore
	close(pipe_A[0]);
	close(pipe_A[1]);
  return 0;
}
// TODO: implement me
	// SEQ_NOT_IMPLEMENTED _MARKER remove this marker to activate seq related testcases
	// AWK_NOT_IMPLEMENTED _MARKER remove this marker to activate awk related testcases
	// GREP_NOT_IMPLEMENTED _MARKER remove this marker to activate grep related testcases
	// OUTPUT_NOT_IMPLEMENTED _MARKER remove this marker to activate output redirection related testcases
