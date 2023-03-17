#include <commands.h>
#include <commandline_parser.h>
#include <stdio.h> // printf()
#include <stdlib.h> // exit()
#include <string.h> // strcpy()


struct job job_buffer[BUF_SIZE]; // buffer to store the jobs
char policy[10] = "fcfs"; // Switch Scheduling Policy

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

int cmd_list(int nargs, char **args)
{
    printf("Total number of jobs in the queue: %d\n",jobCount);

    // Print the menu
    printf("Scheduling Policy: %s.\n",policy);
    printf("Name%*s", MAXJOBNAMELENGTH-4, "");
    printf("CPU_TIME ");
    printf("Pri ");
    printf("Progress\n");

    // print each job information
    int i;
    for (i = 0; i < jobCount; i++) {
        if (job_buffer[i].name != NULL) {
            int jobNameLen = strlen(job_buffer[i].name);
            printf("%s%*s", job_buffer[i].name, MAXJOBNAMELENGTH-jobNameLen, "");
            printf("%d%*s", job_buffer[i].time, 8, "");
            printf("%d%*s", job_buffer[i].priority, 3, "");
            printf("TEMP\n");
        }
    }
    return 0;
}

int cmd_run(int nargs, char **args)
{

    if (nargs != 4)
	{
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}

    // add job info to the job list
    char *jobName = job_buffer[buf_head].name;
    strcpy(jobName, args[1]);
    job_buffer[buf_head].time = atoi(args[2]);
    job_buffer[buf_head].priority = atoi(args[3]);

    jobCount++; // increase the job count
    buf_head++; // Move the buffer head forward
    if (buf_head == BUF_SIZE)
            buf_head = 0;

    printf("Job %s was submitted.\n",jobName);
    printf("Total number of jobs in the queue: %d\n",jobCount);
    printf("Expected waiting time: %d seconds\n", 0);
    printf("Scheduling Policy: %s.\n", policy);

	/* Use execv to run the submitted job in AUbatch */
	printf("\nuse execv to run the job in AUbatch.\n");
	return 0; /* if succeed */
}

int cmd_scheduling(int nargs, char **args){
    
    if(strcmp(args[0],"fcfs\n") == 0){
        strcpy(policy, "fcfs");
    }

    else if(strcmp(args[0],"sjf\n") == 0){
        strcpy(policy,"sjf");
    }

    else{
        printf("Not a scheduling option.\n");
        return 1;
    }

    printf("Scheduling policy is switched to %s. ", policy);
    printf("All the %d waiting jobs have been rescheduled.\n", jobCount);

    return 0;

}

int cmd_quit(int nargs, char **args)
{
	printf("Please display performance information before exiting AUbatch!\n");
	// exit(0);
    return QUITCMD;
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