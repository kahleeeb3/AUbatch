
/*
* Allocates space for the user input, pprompts the user
* for their input, and passes the input to the
* parsecmd() and then dispatches command with
* the cmd_dispatch() function.
*/
void get_input();

/*
* Accepts user input as a string, and parses the input into
* an array of string that will be stored in the args pointer.
* @param cmd		pointer to user input as a string
* @param args		pointer to array of user inpput strings
* @param nargs		pointer to the number of arguments
*/
int parsecmd(char *cmd, char *args[], int *num_args);

/*
* Selests the appropriate funstion to execute
* according to the user input passed by the argument
* if no command is found, returns error.
* @param nargs		number of arguments
*/
int cmd_dispatch(char *args[], int num_args);

/*
* Shows the help menu.
*/
int cmd_helpmenu(int n, char **a);

/*
* Parses and prints the data stored in static const char *helpmenu[]
*/
void showmenu(const char *name, const char *x[]);

/*
* Exits the scheduler thread and prints statistics of finished processes
* NOTE: ALL PROCESSES MUST BE FINISHED BEFORE EXITING. CHECK THIS IS TRUE
* BY USING THE `list` command.
*/
int cmd_quit(int nargs, char **args);