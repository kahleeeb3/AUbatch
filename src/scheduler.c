#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <scheduler.h>

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

void *producer(){
    while( jobs_submitted < TOTAL_JOB_NUM ){
        pthread_mutex_lock(&job_queue_lock); // lock the job queue
        // wait for queue to not be full
        while(jobs_in_queue == QUEUE_SIZE){
            pthread_cond_wait(&job_queue_not_full, &job_queue_lock);
        }
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue since creat_job() is not critical

        struct job new_job = create_job(); // create the job
        
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
        print_job_queue();
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
        jobs_in_queue--; //decrease the job count
        // execv(); //execute the command?
        struct job removed_job = remove_from_queue();
        printf("Thread2: Removed Job: {%s, %d, %d}. Total jobs: %d\n", removed_job.name, 
        removed_job.cpu_time, removed_job.priority, jobs_submitted);
        print_job_queue();
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

// HELPER FUNCTIONS
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

struct job create_job(){
    int min, max, range, random_num;
    struct job new_job; // define new job

    // set the name
    sprintf(new_job.name, "job%d", jobs_submitted);

    //set cpu time
    min = cpu_time_range[0];
    max = cpu_time_range[1];
    range = max - min + 1;
    random_num = min + (rand() % range);

    new_job.cpu_time = random_num;

    // set the priority
    min = priority_range[0];
    max = priority_range[1];
    range = max - min + 1;
    random_num = min + (rand() % range);

    new_job.priority = random_num;

    return new_job;
}

void insert_into_queue(struct job *job_ptr){
    job_queue[job_queue_head] = *job_ptr;
}

struct job remove_from_queue(){
    struct job empty_job = { .name = {0}, .cpu_time = 0, .priority = 0 };
    struct job removed_job = job_queue[job_queue_tail];
    job_queue[job_queue_tail] = empty_job;
    // free(job_queue[job_queue_tail]);
    return removed_job;
}

