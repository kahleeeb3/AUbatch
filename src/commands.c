#include <commands.h>
#include <commandline_parser.h>
#include <stdio.h> // printf()
#include <stdlib.h> // exit()


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

int cmd_run(int nargs, char **args)
{

    if (nargs != 4)
	{
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}

    
    char *jobName  = args[1];
    int executionTime = atoi(args[2]);
    int priority = atoi(args[3]);
    printf("%s, ",jobName);
    printf("%d, ",executionTime);
    printf("%d\n",priority);

	/* Use execv to run the submitted job in AUbatch */
	// printf("use execv to run the job in AUbatch.\n");
	return 0; /* if succeed */
}

int cmd_quit(int nargs, char **args)
{
	printf("Please display performance information before exiting AUbatch!\n");
	exit(0);
}

void showmenu(const char *name, const char *x[])
{
	printf("\n");

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