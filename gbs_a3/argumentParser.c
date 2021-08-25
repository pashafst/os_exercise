#include "argumentParser.h"
#include <stdio.h>
#include <stddef.h>

char **p_argv; // Points to our argument list
int c_argc; // Holds the number of true arguments
int entries; // Holds the size of **p_argv


int isOption(char *option) {
	// Checks if first char is a dash, if not, it surely is no option
	if(*option != '-')
		return 0; // No option

	// We take a look at the next character
	char *c = option + 1;

	while(*c != '\0') { // We iterate through the entire string to find our equal sign
		if(*(c) == '=') // Check for equal sign
			return 1; // Here it is, so it's definitely an option
		c++; // No equal sign, looking further
	}

	return 0; // No valid option found, it's either missing a dash or an equal sign
}

// Checks if the given option is a valid option (meaning, it's an option and holds a valid value)
int isValidOption(char *option) {
	// We check if the given "option" is truly an option
	if(!isOption(option))
		return 0; // Got ya, you're not an option. Bad boy!

	// Good, surely we found a real option
	char *c = option + 1; // Let's skip the dash at the beginning

	while(*c != '=') { // Now we search for the equal sign
		if(*c == '\0') // We didn't find an equal sign but a null character. Yikes, abort
			return 0; // Nope, return 0. Not valid Option
		c++; // Move one character forward and look for other characters owo
	}

	return 1; // We found a valid option UwU
}

// Checking if given argument is really an argument
int isArgument(char *argument)  {
	if(isOption(argument)) // If it's an option it's surely not an argument owo
		return 0; // Nopu, no argument uwu

	return 1; // It is an argument UwU
}

int checkValidity() {
	int arg_count = 0; // Counts the number of real arguments
	int option_found = 0; // Flag for assuring the right order of arguments and options

	for(int i = 0; i < c_argc; i++) { // Iterate through the list of arguments
		if(isArgument(p_argv[i])) {  // Check if given argument is a real argument
			if(option_found) // Woopsie daisy, we found an option earlier, that's against our order of arguments. Abort
				return -1; // Nope uwu

			arg_count++; // We found an argument, so we increase our argument counter
		}
		if(isOption(p_argv[i])) { // Check if given argument is an option
			if(isValidOption(p_argv[i])) { // Is it an option? UwU
				option_found = 1; // IT IS! Set flag for keeping our order
				continue; // Skip the rest
			} else {
				return -1; //  We didn't find a valid argument and no valid option. That's weird, surely it's not valid. Abort uwu
			}
		}
	}

	c_argc = arg_count; // Update the real argument counter

	return 0; // Yup no errors here, it's valid
}

// Initiliaze our argumentParser
int initArgumentParser(int argc, char* argv[]) {
	p_argv = argv; // Copy value
	c_argc = argc; // Copy value
	entries = argc; // Copy value

	return checkValidity(); // Check if arguments are valid
}

// Return name of the command
char* getCommand(void) {
	return p_argv[0]; // It's usually the first entry
}

// Returns the number of arguments
int getNumberOfArguments(void) {
	return c_argc - 1; // Returns the number as an index number so 5 arguments -> return max index 4
}

// Return the name of the argument at index
char* getArgument(int index) {
	// Skip the command
	index++;

	if(index >= entries) // We surely can't access greater indices than we have entries
		return NULL; // UwU

	if(isArgument(p_argv[index])) // Checks if the argument at index is a real argument
		return p_argv[index]; // Yup real argument, return argument
	else
		return NULL; // Nope, no Argument UwU
}

// Return the value of the given option with the name keyName
char* getValueForOption(char* keyName) {
	char *option = NULL; // If we find a valid option with given name keyName, we save it in this variable
	for(int i = 0; i < entries; i++) { // Iterate through our list
		if(option != NULL)  // We already found our valid option. Stopping the loop
			break; // Stahp
		if( isValidOption(p_argv[i]) ) { // Valid option found
			option = p_argv[i]; // Save the valid option
			char *c = p_argv[i] + 1; // Skip the dash
			char *ck = keyName; // Copies the pointer pointing at the char array to perform char comparison
			while(*c != '\0' && *ck != '\0') { // Iterate through string
				if(*ck != *c) { // Checks if the characters differ
					option = NULL; // They do, return NULL
					break; // Stop the loop
				}
				c++; // They are the same, jump one forward
				ck++; // They are the same, jump one forward
			}
		}
	}

  // We didn't find a valid option :(
	if(option == NULL)
		return NULL;

  // Move forward to the first character after the equal sign, return pointer
	char *c = option;
	while(*c != '=') { // Fast forward
		c++;
	}
	c++; // Point to the char after the equal sign
	return c; // Return magic
}
