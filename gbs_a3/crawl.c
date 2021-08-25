// Standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

// File and match operations
#include <sys/stat.h>
#include <regex.h>
#include <fnmatch.h>

// uwu
#include "argumentParser.h"

// Beenden mit Fehlermeldung
void die(const char *msg) { 
  perror(msg);
  exit(1);
}

// Print path to file 
void print_path(char *filename, char *path) {
  printf("%s/%s\n", path, filename); // then print the name of file
}

int containsLine(char *path, char *file, regex_t *line_regex) {
	FILE *f;
	char line[1024];
	int linecount = 1;
	char epath[500] = {0};
	strcpy(epath, path);
	strcat(epath, "/");
	strcat(epath, file);

	f = fopen(epath, "r");

	if(f == 0) {
		fclose(f);
		return 0;
	}

	while((fgets(line, 1024, f)) != NULL) {
		linecount++; 

		if(regexec(line_regex, line, 0, NULL, 0) == 0) // Do we have a match?
			printf("%s/%s:%d:%s", path, file, linecount, line);	 // Oh boi, we have!
	}

	fclose(f); // Shutting the door and saying "Goodbye!"
	return 0; // "Goodbye!"
}

// We crawl for truth
static void crawl(char * path, int depth, const char * pattern, char type, int sizeMode, off_t size, regex_t *reg) {
  if(sizeMode == -1 && type != 'f' && *pattern == '0' && reg == NULL)   // Print current directory when dying and if we are allowed to
	  printf("%s\n", path);

  if(depth == 0)
	  return;

  DIR * dir = opendir(path);
  struct dirent *entry;

  if(!dir) 
	  die("CAN'T OPEN DIR");

  struct stat stats;
  if(sizeMode != -1 && lstat(path, &stats) == 0 && reg == NULL) { // Checks if we need to check the size of the current directory. If so,
	      // we check the size and
	      if((sizeMode == 2 && stats.st_size == size) || (sizeMode == 1 && stats.st_size < size) || (sizeMode == 0 && stats.st_size >= size)) 
				printf("%s\n", path);	 // print the directory, if it meets the right criteria
  }

  for(entry = readdir(dir); entry != 0; entry = readdir(dir)) { 
      // We surely don't want to print those
      if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
	      continue;

      // Checking if we are allowed to print files, if not, we don't compute them
      if(type != 'd' && entry->d_type == 8) {
	      char epath[500] = {0}; // Will hold the entire path of the files
	      strcpy(epath, path); // copy our current path
	      strcat(epath, "/"); // Appending the slash
	      strcat(epath, entry->d_name); // Aaand the filename

	      // Time to get woozy
	      if(lstat(epath, &stats) == 0) {  // We check if we can get informations about our file
	      // We check the size
	      if(sizeMode == -1 || (sizeMode == 2 && stats.st_size == size) || (sizeMode == 1 && stats.st_size < size) || (sizeMode == 0 && stats.st_size >= size)) {
		if(*pattern == '0') { // We don't need to check the filename
			if(reg != NULL)  // We need to check the content of our file
				containsLine(path, entry->d_name, reg); // Do the magic and look for the given regex expression
			else 
			      	print_path(entry->d_name, path); // We don't need regex, thus we just print the path of the file, since it matches our size criteria
		} else { // We need to check the filename
			if(fnmatch(pattern, entry->d_name, FNM_PATHNAME) == 0) { // We check the filename
			if(reg != NULL)  // We need to check the content of our file
				containsLine(path, entry->d_name, reg);
			else 
			        print_path(entry->d_name, path); // We don't need to check the content
			      
			}
		  }
	      }} // lstat and sizemode if brackets
	      continue; // Save a couple of steps 
      }

      // Are we dealing with a directory?
      if(entry->d_type == 4) {
	      char epath[500] = {0}; // Extended path
	      strcpy(epath, path);
	      strcat(epath, "/");
	      strcat(epath, entry->d_name);
	      crawl(epath, depth-1, pattern, type, sizeMode, size, reg); // Go into next directory with one depth subtracted
      }
  }
  closedir(dir);  // We are done, we shut the door and say goodbye. Thank you so much for reading this, have a nice day and thank you for your work
}

int main(int argc, char *argv[]) {

  if(initArgumentParser(argc, argv)) {
    die("Falsche Benutzung!");
  }
  // Some standard values
  int depth = 100; 
  int sizeMode = -1;
  int size = 0; 
  char *pattern = "0";
  unsigned char type = '*';

  // Preparing our regex
  char *line;
  regex_t reg;
  int regset = 0; // Is regex set?

  // Grabbing values
  if(getValueForOption("maxdepth") != NULL) {
    	depth = atoi(getValueForOption("maxdepth"));
  }

  if(getValueForOption("type") != NULL) {
	type = getValueForOption("type")[0];
  }

  if(getValueForOption("name") != NULL) {
	pattern = getValueForOption("name");
  }

  if(getValueForOption("line") != NULL) {
	line = getValueForOption("line");
  	regcomp(&reg, line, REG_EXTENDED);
  	regset = 1;
  }

  if(getValueForOption("size") != NULL) {
    size = atoi(getValueForOption("size"));
    if(getValueForOption("size")[0] == '+') {
      sizeMode = 0;
    }
    if(getValueForOption("size")[0] == '-') {
      sizeMode = 1;
      size *= -1; // Negative value, so we make it negative negative!
    }
    if(sizeMode == -1)
      sizeMode = 2;
  }

  for(int i = 0; i < getNumberOfArguments(); i++) {
	  if(regset)
  		crawl(getArgument(i), depth, pattern, type, sizeMode, size, &reg);
	  else
  		crawl(getArgument(i), depth, pattern, type, sizeMode, size, NULL);
  }
  // Freeing the memory of our regist...regex
  if(regset)
	  regfree(&reg);

  return 0; // You made it \o/ 
}
