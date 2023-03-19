// contains the scheduler and dispatcher modules

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <scheduler.h>
#include <sys/wait.h> // wait function
#include <commandline.h>

int cpu_time_range[] = {1,10};
int priority_range[] = {1,10};
char policy[10] = "fcfs"; // Switch Scheduling Policy

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

// SCHEDULER MODULES

void *scheduler(){
    while( jobs_submitted < TOTAL_JOB_NUM ){
        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be full
        while(jobs_in_queue == QUEUE_SIZE){
            printf("Buffer full. Please Wait.\n");
            pthread_cond_wait(&job_queue_not_full, &job_queue_lock);
        }
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue since creat_job() is not critical

        get_input();
        pthread_cond_signal(&job_queue_not_empty); // tell execution process that the buffer isnt empty
        // there is another lock within cmd_run so unlock here
    }

    return 0;
}

struct job create_job(char *args[]){
    struct job new_job; // define new job
    strcpy(new_job.name, args[1]); // set the name
    new_job.cpu_time = atoi(args[2]); //set cpu time
    new_job.priority = atoi(args[3]); // set the priority
    return new_job;
}

int insert_into_queue(struct job *job_ptr){
    job_queue[job_queue_head] = *job_ptr;
    return 0;
}

int add_job(int nargs, char **args){
    // CREATE THE JOB
    struct job new_job = create_job(args);

    // INSERT INTO QUEUE
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
    pthread_mutex_unlock(&job_queue_lock); // unlock the job queue
    return 0;
}

int cmd_list(int nargs, char **args)
{
    printf("Total number of jobs in the queue: %d\n",jobs_in_queue);

    // Print the menu
    printf("Scheduling Policy: %s.\n",policy);
    printf("Name%*s", MAX_JOB_NAME_LENGTH-4, "");
    printf("CPU_TIME ");
    printf("Pri ");
    printf("Progress\n");

    // print each job information
    int i;
    for (i = 0; i < jobs_in_queue; i++) {
        if (job_queue[i].name != NULL) {
            int jobNameLen = strlen(job_queue[i].name);
            printf("%s%*s", job_queue[i].name, MAX_JOB_NAME_LENGTH-jobNameLen, "");
            printf("%d%*s", job_queue[i].cpu_time, 8, "");
            printf("%d%*s", job_queue[i].priority, 3, "");
            printf("TEMP\n");
        }
    }
    return 0;
}

int automated_input(int nargs, char **args){
    int i, min, max, range, random_cpu_time, random_priority;
    for(i = 0; i< TOTAL_JOB_NUM; i++){
        
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

        // GET USER INPUT AS A STRING
        char *temp_cmd;
        size_t cmd_size = MAX_CMD;
        temp_cmd = (char *)malloc(cmd_size * sizeof(char));
        sprintf(temp_cmd, "run process %d %d\n", random_cpu_time , random_priority);

        // PARSE THE USER INPUT
        char *args[MAX_ARGS];
        int num_args; // number of argumenst
        parsecmd(temp_cmd, args, &num_args);

        // DISPATCH THE USER COMMAND
        cmd_dispatch(args, num_args);

    }
    

    pthread_cond_signal(&job_queue_not_empty); // tell execution process that the buffer isnt empty
    return 0;
}

// EXECUTOR MODULES
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
        printf("\nThread2: Executing Job: {%s, %d, %d}. Total jobs: %d\n", removed_job.name, 
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

void run_cmd(char *cmd){
    char *args[MAX_ARGS];
    int nargs;
    parsecmd(cmd, args, &nargs);
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

        // if (WIFEXITED(status)) { // Child process terminated normally
        //     printf("Child process terminated with status %d.\n", WEXITSTATUS(status));
        // } 
        // else if (WIFSIGNALED(status)) { // Child process terminated due to a signal
        //     printf("Child process terminated due to signal %d.\n", WTERMSIG(status));
        // } 
        // else { // Something else happened
        //     printf("Child process terminated abnormally.\n");
        // }
        
    }
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


