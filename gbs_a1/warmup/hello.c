#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

char string[] = "Hallo Welt!";

int main() {
  printf("%s\n", string);
  printf("user: %d\n", getuid());
  printf("group: %d\n", getgid());
  return 0;
}
