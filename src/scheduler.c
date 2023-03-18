// contains the scheduler and dispatcher modules

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <scheduler.h>
#include <sys/wait.h> // wait function

int cpu_time_range[] = {1,10};
int priority_range[] = {1,10};

/* Global shared variables */
u_int job_queue_head = 0;
u_int job_queue_tail = 0;
u_int jobs_in_queue = 0;
u_int jobs_submitted = 0;

struct job {
    char name[MAX_JOB_NAME_LENGTH]; // job name
    int cpu_time;                   // time required to complete the job
    int priority;                   // job priority
};

struct job job_queue[QUEUE_SIZE];

int parsecmd(char *cmd, char *args[]) {
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
    while(nargs < MAX_ARGS){
        args[nargs++] = NULL;
    }

    return 0;
}

// HELPER FUNCTIONS
void automated_input(char *cmd){
    int min, max, range, random_cpu_time, random_priority;

    //set cpu time
    min = cpu_time_range[0];
    max = cpu_time_range[1];
    range = max - min + 1;
    random_cpu_time = min + (rand() % range);

    // set the priority
    min = priority_range[0];
    max = priority_range[1];
    range = max - min + 1;
    random_priority = min + (rand() % range);

    // set the name
    sprintf(cmd, "run process %d %d\n", random_cpu_time , random_priority);


}

void run_cmd(char *cmd)
{
    char *args[MAX_ARGS];
    parsecmd(cmd, args);
    pid_t pid = fork();  // create a child process

    if (pid == -1) {
        perror("fork");  // handle fork errors
        exit(1);
    } 
    else if (pid == 0) {
        // child process
        execv(args[0], args);  // execute the command with the given arguments
        perror("execv");  // handle execv errors
        exit(1);
    } 
    else {
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) { // Child process terminated normally
            printf("Child process terminated with status %d.\n", WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) { // Child process terminated due to a signal
            printf("Child process terminated due to signal %d.\n", WTERMSIG(status));
        } 
        else { // Something else happened
            printf("Child process terminated abnormally.\n");
        }
        
    }
}

void print_job_queue(){
    printf("============\n");
    int i;
    for (i = 0; i < QUEUE_SIZE; i++) {
        if (job_queue[i].name[0] == '\0') {
            printf("NONE");
        }
        printf("%s", job_queue[i].name);
        printf(" %d", job_queue[i].cpu_time);
        printf(" %d\n", job_queue[i].priority);
    }
    printf("\n");
}

struct job create_job(char *args[]){
    struct job new_job; // define new job
    strcpy(new_job.name, args[1]); // set the name
    new_job.cpu_time = atoi(args[2]); //set cpu time
    new_job.priority = atoi(args[3]); // set the priority
    return new_job;
}

void insert_into_queue(struct job *job_ptr){
    job_queue[job_queue_head] = *job_ptr;
}

struct job remove_from_queue(){
    // remove from the queue
    struct job removed_job = job_queue[job_queue_tail]; // select the job to remove
    struct job empty_job = { .name = {0}, .cpu_time = 0, .priority = 0 };
    job_queue[job_queue_tail] = empty_job;
    
    // run the command
    // char *my_args[3];
    // my_args[0] = "process";
    // char my_str[6];
    // sprintf(my_str, "%d", removed_job.cpu_time);
    // my_args[1] = my_str;
    // my_args[2] = NULL;
    
    return removed_job;
}

void *scheduler(){
    while( jobs_submitted < TOTAL_JOB_NUM ){
        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be full
        while(jobs_in_queue == QUEUE_SIZE){
            pthread_cond_wait(&job_queue_not_full, &job_queue_lock);
        }
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue since creat_job() is not critical

        // GET USER INPUT AS A STRING
        char *temp_cmd;
        temp_cmd = (char *)malloc(MAX_CMD * sizeof(char));
        automated_input(temp_cmd); // simulate a user input of "run process x x"

        // PARSE THE USER INPUT
        char *args[MAX_ARGS];
        parsecmd(temp_cmd, args);
        
        // create the job
        struct job new_job = create_job(args);
        
        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        insert_into_queue(&new_job); // add job to queue
        jobs_in_queue++; // increase the job count
        jobs_submitted++;
        job_queue_head++; // move head forward
        if(job_queue_head == QUEUE_SIZE){
            job_queue_head = 0;
        }
        printf("Thread1: Added Job: {%s, %d, %d}. Total jobs: %d\n", new_job.name, 
        new_job.cpu_time, new_job.priority, jobs_submitted);
        // print_job_queue();
        pthread_cond_signal(&job_queue_not_empty); // tell execution process that the buffer isnt empty
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue
        sleep(ARRIVAL_RATE);
    }

    return 0;
}

void *executor(){
    while (jobs_submitted < TOTAL_JOB_NUM){

        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be empty
        while(jobs_in_queue == 0){
            pthread_cond_wait(&job_queue_not_empty, &job_queue_lock);
        }

        // REMOVE JOB FROM QUEUE
        jobs_in_queue--; //decrease the job count
        struct job removed_job = remove_from_queue();
        printf("Thread2: Executing Job: {%s, %d, %d}. Total jobs: %d\n", removed_job.name, 
        removed_job.cpu_time, removed_job.priority, jobs_submitted);
        // print_job_queue();

        // EXECUTE JOB
        char *my_cmd;
        my_cmd = (char *)malloc(MAX_CMD * sizeof(char));
        sprintf(my_cmd, "%s %d\n", removed_job.name , removed_job.cpu_time);
        run_cmd(my_cmd);

        sleep(SERVICE_RATE); // sleep for service rate
        job_queue_tail++;
        if(job_queue_tail == QUEUE_SIZE){
            job_queue_tail = 0;
        }
        pthread_cond_signal(&job_queue_not_full);
        pthread_mutex_unlock(&job_queue_lock);

    }

    return 0;
}