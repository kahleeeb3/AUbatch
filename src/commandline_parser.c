/*
 * Documentation:
 * getline(): https://man7.org/linux/man-pages/man3/getline.3.html
 * strtok_r(): https://man7.org/linux/man-pages/man3/strtok.3.html
 * strcmp(): https://man7.org/linux/man-pages/man3/strcmp.3.html
 * 
*/
#include <stdio.h> // printf()
#include <commandline_parser.h>
#include <commands.h>
// #include <sys/types.h>
#include <stdlib.h> // malloc(), exit()
#include <assert.h> // assert()
#include <string.h> // strtok(), strcmp()

static struct
{
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{"?\n", cmd_helpmenu},
	{"h\n", cmd_helpmenu},
	{"help\n", cmd_helpmenu},
	{"r", cmd_run},
	{"run", cmd_run},
	{"q\n", cmd_quit},
	{"quit\n", cmd_quit},
	/* Please add more operations below. */
	{NULL, NULL}};


int cmd_dispatch(char *cmd){

	// (1) split the user input into an argument array
	char *word; // pointer to the word value
	char *context; // ensures correct context during threading
	int nargs = 0; // number of arguments
	char *args[MAXMENUARGS]; // array of arguments

	word = strtok_r(cmd, " ", &context);

	while (word != NULL) {

		// If too many arguments were passed
		if (nargs >= MAXMENUARGS)
		{
			printf("Command line has too many words\n");
			printf("Too many arguments! The maximum number of allowed arguments is %d\n", MAXMENUARGS);
			return E2BIG;
		}
		args[nargs++] = word; // add word to the argument list
		word = strtok_r(NULL, " ", &context); // NULL to continue tokenizing the same string
    }

	// if no arguments were passed
	if (nargs == 0){
		return EINVAL;
	}
	// (2) Run the command associated with the user input
	// iterate over all elements in the command table until the cmd name is NULL
	int i, result;
	for (i = 0; cmdtable[i].name; i++){
		// if the name is not NULL and mathes the argument given by the user
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)){
			// terminate program if cmd doesn't have an associated function
			assert(cmdtable[i].func != NULL);

			// run the associated function
			result = cmdtable[i].func(nargs, args);
			return result;
		}
	}
	printf("Command (%s) not found\n", word);
	return EINVAL;
}