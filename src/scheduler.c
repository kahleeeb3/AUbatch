/* 
 * COMP7500
 * Project 3: scheduler.c 
 *
 * Conatains modules for changing the Job Queue, and dispatching/executing jobs.
 * 
 * Notes:
 * In the header file, we define a QUEUE_SIZE. However, this is a linked list so
 * it's not necissary to have a max quantity. We define this to simulate a restriction
 * on a buffer size even though one does not truly exist.
 * 
 * For more information on what each of the individual functions does, please
 * look at the scheduler.h file in the `include/` folder.
 * 
 * Modified by Caleb Powell
 * Department of Computer Science and Software Engineering
 * Auburn University
 * Mar. 19, 2023.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <scheduler.h>
#include <sys/wait.h> // wait function
#include <commandline.h>
#include <time.h>
#include <dirent.h>

// Global shared variables
int cpu_time_range[2] = {1,10};
int priority_range[2] = {1,10};
char policy[10] = "fcfs"; // Switch Scheduling Policy
int total_num_jobs = 5;
u_int jobs_in_queue = 0;
u_int jobs_submitted = 0;
struct node* job_list = NULL;

// *** THIS SECTION DEFINES MODULES USED BY THE SCHEDULER THREAD *** //

void *scheduler(){
    while( jobs_submitted < total_num_jobs ){
        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be full
        while(jobs_in_queue == QUEUE_SIZE){
            printf("Buffer full. Please Wait.\n");
            pthread_cond_wait(&job_queue_not_full, &job_queue_lock);
        }
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue since creat_job() is not critical
        get_input();
    }

    return 0;
}

int add_job(int nargs, char **args){
    // CREATE THE JOB
    struct job new_job = create_job(args);

    pthread_mutex_lock(&job_queue_lock); // lock the job queue

    // INSERT INTO QUEUE
    insert_into_queue(new_job, &job_list); // add job to queue

    jobs_in_queue++; // increase the job count
    jobs_submitted++;

    pthread_cond_signal(&job_queue_not_empty); // tell execution process that the buffer isnt empty
    pthread_mutex_unlock(&job_queue_lock); // unlock the job queue

    // printf("Thread1: Added Job: {%s, %d, %d}. Total jobs: %d\n", new_job.name, 
    // new_job.cpu_time, new_job.priority, jobs_submitted);
    printf("Job %s was submitted.\n",new_job.name);
    printf("Total number of jobs in the queue: %d\n",jobs_in_queue);
    printf("Expected waiting time: %d seconds\n", new_job.wait_time);
    printf("Scheduling Policy: %s.\n", policy);
    return 0;
}

struct job create_job(char *args[]){

    // define new job
    struct job new_job;

    strcpy(new_job.name, args[1]); // set the name
    new_job.cpu_time = atoi(args[2]); //set cpu time
    new_job.priority = atoi(args[3]); // set the priority
    time(&new_job.start_time); // set the time of creation

    // get wait time
    int sum = 0;
    struct node* current_node = job_list;

    if(strcmp(policy, "priority") == 0){
        while (current_node != NULL && current_node->job_data.priority < new_job.priority) {
            sum += current_node->job_data.cpu_time;
            current_node = current_node->next;
        }
    }

    else if(strcmp(policy, "sjf") == 0){
        while (current_node != NULL && current_node->job_data.cpu_time< new_job.cpu_time) {
            sum += current_node->job_data.cpu_time;
            current_node = current_node->next;
        }
    }

    if(strcmp(policy, "fcfs") == 0){
        while (current_node != NULL && current_node->job_data.start_time < new_job.start_time) {
            sum += current_node->job_data.cpu_time;
            current_node = current_node->next;
        }
    }
    

    new_job.wait_time = sum;
    return new_job;
}

int insert_into_queue(struct job job_data, struct node** list_head){
    
    struct node* new_node = (struct node*) malloc(sizeof(struct node));
    new_node->job_data = job_data;
    new_node->prev = NULL;
    new_node->next = NULL;
    
    // IF THE POLICY IS PRIORITY, LINK ELEMENTS ACCORDING TO THEIR PRIORITY NUMBER
    if(strcmp(policy, "priority") == 0){
        if (*list_head == NULL) {
            *list_head = new_node;
        }
        else if (job_data.priority < (*list_head)->job_data.priority) {
            new_node->next = *list_head;
            (*list_head)->prev = new_node;
            *list_head = new_node;
        }
        else {
            struct node* curr_node = *list_head;
            while (curr_node->next != NULL && curr_node->next->job_data.priority <= job_data.priority) {
                curr_node = curr_node->next;
            }
            new_node->next = curr_node->next;
            new_node->prev = curr_node;
            if (curr_node->next != NULL) {
                curr_node->next->prev = new_node;
            }
            curr_node->next = new_node;
        }
    }

    // IF THE POLICY IS SHORTEST JOB FIRST
    else if(strcmp(policy, "sjf") == 0){
        if (*list_head == NULL) {
            *list_head = new_node;
        }
        else if (job_data.cpu_time < (*list_head)->job_data.cpu_time) {
            new_node->next = *list_head;
            (*list_head)->prev = new_node;
            *list_head = new_node;
        }
        else {
            struct node* curr_node = *list_head;
            while (curr_node->next != NULL && curr_node->next->job_data.cpu_time <= job_data.cpu_time) {
                curr_node = curr_node->next;
            }
            new_node->next = curr_node->next;
            new_node->prev = curr_node;
            if (curr_node->next != NULL) {
                curr_node->next->prev = new_node;
            }
            curr_node->next = new_node;
        }
    }

    // IF THE POLICY IS FIRST COME FIRST SERVE
    else if(strcmp(policy, "fcfs") == 0){
        if (*list_head == NULL) {
            *list_head = new_node;
        }
        else if (job_data.start_time < (*list_head)->job_data.start_time) {
            new_node->next = *list_head;
            (*list_head)->prev = new_node;
            *list_head = new_node;
        }
        else {
            struct node* curr_node = *list_head;
            while (curr_node->next != NULL && curr_node->next->job_data.start_time <= job_data.start_time) {
                curr_node = curr_node->next;
            }
            new_node->next = curr_node->next;
            new_node->prev = curr_node;
            if (curr_node->next != NULL) {
                curr_node->next->prev = new_node;
            }
            curr_node->next = new_node;
        }
    }


    return 0;
}

int cmd_scheduling(int nargs, char **args){
    
    if(strcmp(args[0],"fcfs\n") == 0){
        strcpy(policy, "fcfs");
    }

    else if(strcmp(args[0],"sjf\n") == 0){
        strcpy(policy,"sjf");
    }

    else if(strcmp(args[0],"priority\n") == 0){
        strcpy(policy,"priority");
    }

    else{
        printf("Not a scheduling option.\n");
        return 1;
    }

    printf("Scheduling policy is switched to %s. ", policy);
    printf("All the %d waiting jobs have been rescheduled.\n", jobs_in_queue);

    return 0;

}

int cmd_list(int nargs, char **args)
{
    printf("Total number of jobs in the queue: %d\n",jobs_in_queue);
    printf("Scheduling Policy: %s.\n",policy);
    printf("Name\tCPU_Time\tPri\tArrival_time\n");

    while (job_list != NULL) {

        // print the time
        char time_string[9];
        strftime(time_string, sizeof(time_string), "%H:%M:%S", localtime(&(job_list->job_data.start_time)));
        
        printf("%s\t%d\t\t%d\t%s\n", job_list->job_data.name, job_list->job_data.cpu_time, job_list->job_data.priority, time_string);
        job_list = job_list->next;
    }

    return 0;
}

int automated_input(int nargs, char **args){
    // test <benchmark> <policy> <num_of_jobs> <priority_levels> <min_CPU_time> <max_CPU_time>
    // test mybenchmark fcfs 5 3 1 10

    if(nargs != 7){
        printf("try again loser\n");
    }

    char benchmark[10];
    strcpy(benchmark, args[1]);
    strcpy(policy, args[2]);
    total_num_jobs = atoi(args[3]);
    priority_range[1] = atoi(args[4]);
    cpu_time_range[0] = atoi(args[5]);
    cpu_time_range[1] = atoi(args[6]);
    
    int i, min, max, range, random_cpu_time, random_priority;
    for(i = 0; i< total_num_jobs; i++){
        
        printf("\n");
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

        // get the cmd string
        char temp_cmd[MAX_CMD];
        sprintf(temp_cmd, "run process %d %d\n", random_cpu_time , random_priority);

        // PARSE THE USER INPUT
        char *args[MAX_ARGS];
        int num_args; // number of argumenst
        parsecmd(temp_cmd, args, &num_args);

        // DISPATCH THE USER COMMAND
        cmd_dispatch(args, num_args);

        if(i > 1){
            pthread_mutex_lock(&job_queue_lock); // lock the job queue
            // wait for queue to not be full
            while(jobs_in_queue == QUEUE_SIZE){
                printf("Buffer full. Please Wait.\n");
                pthread_cond_wait(&job_queue_not_full, &job_queue_lock);
            }
            pthread_mutex_unlock(&job_queue_lock); // unlock the job queue since creat_job() is not critical
        }
        sleep(ARRIVAL_RATE); // Simulate an arrival time

    }    

    return 0;
}

// *** THIS SECTION DEFINES MODULES USED BY THE EXECUTOR THREAD *** //
void *executor(){
    int jobs_removed = 0;
    while (jobs_removed < total_num_jobs){

        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be empty
        while(jobs_in_queue == 0){
            pthread_cond_wait(&job_queue_not_empty, &job_queue_lock);
        }

        // REMOVE JOB FROM QUEUE
        jobs_in_queue--; //decrease the job count
        struct job removed_job = remove_from_queue();

        pthread_cond_signal(&job_queue_not_full);
        pthread_mutex_unlock(&job_queue_lock);

        // printf("\nThread2: Executing Job: {%s, %d, %d}. Total jobs: %d\n", removed_job.name, 
        // removed_job.cpu_time, removed_job.priority, jobs_submitted);
        // print_job_queue();

        // EXECUTE JOB
        char *my_cmd;
        my_cmd = (char *)malloc(MAX_CMD * sizeof(char));
        sprintf(my_cmd, "%s %d\n", removed_job.name , removed_job.cpu_time);
        run_cmd(my_cmd);
        time(&removed_job.finish_time); // mark the time finish
        export_data(removed_job, jobs_removed);
        jobs_removed++;
        // sleep(SERVICE_RATE); // sleep for service rate
    }

    return 0;
}

struct job remove_from_queue(){

    struct job removed_job;

    if (job_list == NULL) {
        // the queue is empty, there is nothing to remove
        return removed_job;
    }

    struct node* first_node = job_list;
    removed_job = first_node->job_data;
    job_list = first_node->next;
    free(first_node);

    return removed_job;
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

void export_data(struct job my_job, int job_number){
    FILE *fp;

    char filename[50];
    sprintf(filename, "data/job_%d.txt", job_number);

    fp = fopen(filename, "w");
    fprintf(fp, "%s,%d,%d,%ld,%ld,%d\n", my_job.name, my_job.cpu_time, my_job.priority, my_job.start_time, my_job.finish_time, my_job.wait_time);
    fclose(fp);

    // printf("saving file data/job_%d.txt\n", job_number);
}

int show_stats(){

    // printf("Getting data\n");
    printf("\n");

    struct job jobs[total_num_jobs];
    int num_jobs = 0;
    
    // Open the data directory for reading
    DIR* dir = opendir("data");
    if (!dir) {
        perror("Unable to open directory");
        return 1;
    }

    // Iterate over all files in the data directory
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        // Check if the file name starts with "job_" and ends with ".txt"
        if (strstr(entry->d_name, "job_") == entry->d_name && strstr(entry->d_name, ".txt") != NULL) {
            // Construct the file path
            char filepath[100];
            snprintf(filepath, sizeof(filepath), "data/%s", entry->d_name);
            
            // Read the job from the file and store it in the jobs array
            if (num_jobs < total_num_jobs) {
                read_job_from_file(filepath, &jobs[num_jobs]);
                num_jobs++;
            }
        }
    }


   // Close the data directory
    closedir(dir);
    
    // GET STATS

    int i;
    double avg_turnaround_time = 0;
    double avg_cpu_time = 0;
    double avg_wait_time = 0;

    for (i = 0; i < num_jobs; i++) {
        avg_turnaround_time += difftime(jobs[i].finish_time, jobs[i].start_time);
        avg_cpu_time += jobs[i].cpu_time;
        avg_wait_time += jobs[i].wait_time;        
    }

    // get average
    avg_turnaround_time /= num_jobs;
    avg_cpu_time /= num_jobs;
    avg_wait_time /= num_jobs;
    double throughput = num_jobs / (double) difftime(jobs[num_jobs-1].finish_time, jobs[0].start_time);
    

    // print the stats
    printf("Total number of jobs submitted: %d\n", num_jobs);
    printf("Average turnaround time: %.2f seconds\n", avg_turnaround_time);
    printf("Average CPU time: %.2f seconds\n", avg_cpu_time);
    printf("Average waiting time: %.2f seconds\n", avg_wait_time);
    printf("Throughput: %.2f jobs per second\n", throughput);

    return 0;

    
}

void read_job_from_file(const char* filename, struct job* job) {
    // Open the file for reading
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return;
    }
    
    // Read the job data from the file
    fscanf(file, "%[^,],%d,%d,%ld,%ld,%d\n", job->name, &job->cpu_time, &job->priority, &job->start_time, &job->finish_time, &job->wait_time);

    // Close the file
    fclose(file);
}