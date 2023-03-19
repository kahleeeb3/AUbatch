#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <scheduler.h>
#include <sys/wait.h> // wait function
#include <commandline.h>
#include <assert.h>

static struct
{
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{"?\n", cmd_helpmenu},
	{"h\n", cmd_helpmenu},
	{"help\n", cmd_helpmenu},
    {"q\n", cmd_quit},
	{"quit\n", cmd_quit},
    {"r", add_job},
	{"run", add_job},
    {"list\n", cmd_list},
	{"l\n", cmd_list},
    {"t\n", automated_input},
	/* Please add more operations below. */
	{NULL, NULL}};

static const char *helpmenu[] = {
	"run <job> <time> <pri>: submit a job named <job>,\n\
                        execution time is <time>,\n\
                        priority is <pri>.",
	"list: display the job status",
    "fcfs: change the scheduling policy to FCFS.",
    "sjf: change the scheduling policy to SJF.",
    "priority: change the scheduling policy to priority.",
    "test <benchmark> <policy> <num_of_jobs> <priority_levels>\n\
     <min_CPU_time> <max_CPU_time>",
    "quit: exit AUbatch",
	/* Please add more menu options below */
	NULL};

void get_input(){

    // GET USER INPUT AS A STRING
    char *temp_cmd;
    size_t cmd_size = MAX_CMD;
    temp_cmd = (char *)malloc(cmd_size * sizeof(char));
    // automated_input(temp_cmd); // simulate a user input of "run process x x"
    printf("\n>");
    getline(&temp_cmd, &cmd_size, stdin);

    // PARSE THE USER INPUT
    char *args[MAX_ARGS];
    int num_args; // number of argumenst
    parsecmd(temp_cmd, args, &num_args);

    // DISPATCH THE USER COMMAND
    cmd_dispatch(args, num_args);
}

int parsecmd(char *cmd, char *args[], int *num_args) {
    // (1) split the user input into an argument array
	char *word; // pointer to the word value
	char *context; // ensures correct context during threading
	int nargs = 0; // number of arguments


	for (word = strtok_r(cmd, " ", &context);
		 word != NULL;
		 word = strtok_r(NULL, " ", &context))
	{

		if (nargs >= MAX_ARGS)
		{
			printf("Command line has too many words\n");
			return 1;
		}
		args[nargs++] = word;
	}

	// if no arguments were passed
	if (nargs == 0){
		return 1;
	}

    // if there is some empty space at end, set null
    *num_args = nargs; // store the number of args
    while(nargs < MAX_ARGS){
        args[nargs++] = NULL;
    }

    return 0;
}

int cmd_dispatch(char *args[], int num_args){
    int i, result;
    for (i = 0; cmdtable[i].name; i++)
	{
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name))
		{
			assert(cmdtable[i].func != NULL);

			/*Qin: Call function through the cmd_table */
			result = cmdtable[i].func(num_args, args);
			return result;
		}
	}

	printf("%s: Command not found\n", args[0]);
	return 1;
}

void showmenu(const char *name, const char *x[])
{
	int i;
    for(i = 0; x[i]; i++){
        printf("%s\n",x[i]);
    }
}

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu("AUbatch help menu", helpmenu);
	return 0;
}

int cmd_quit(int nargs, char **args)
{
	printf("Please display performance information before exiting AUbatch!\n");
	exit(0);
    // return QUITCMD;
}