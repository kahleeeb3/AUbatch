/*
 *
 * pthread library documentation: 
 * https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
 * 
 * execv(): https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/execv.htm
 *
 */

// #include <commandline_parser.h>
#include <aubatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX_JOB_NAME_LENGTH 10 // maximum length of job name
#define TOTAL_JOB_NUM 5 // total number of jobs to submit 
#define QUEUE_SIZE 10 // size of job queue
#define ARRIVAL_RATE 1 //number of jobs submitted every second
#define SERVICE_RATE 1 //number of jobs submitted every second

void *producer();
void *executor();

pthread_mutex_t job_queue_lock;   // Lock for critical sections
pthread_cond_t job_queue_not_full;  // Condition variable for not full queue
pthread_cond_t job_queue_not_empty; // Condition variable for non empty queue

int cpu_time_range[] = {1,10};
int priority_range[] = {1,10};

/* Global shared variables */
u_int job_queue_head;
u_int job_queue_tail;
u_int jobs_in_queue;
u_int jobs_submitted;

struct job {
    char name[MAX_JOB_NAME_LENGTH]; // job name
    int cpu_time;                   // time required to complete the job
    int priority;                   // job priority
};

struct job job_queue[QUEUE_SIZE];

int main(){
    srand(time(NULL)); // seed the random number generator with the current time
    // commandline(); // run user interface
    int iret1, iret2; // thread return values
    pthread_t producer_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Producer Thread";
    char *message2 = "Executor Thread";

    jobs_in_queue = 0; // current number of jobs in queue
    jobs_submitted = 0; // total number of jobs submitted
    job_queue_head = 0; // points to start of queue
    job_queue_tail = 0; // next empty space in queue
    
    // Create independent threads each of which will execute function
    iret1 = pthread_create(&producer_thread, NULL, producer, (void *)message1);
    iret2 = pthread_create(&executor_thread, NULL, executor, (void *)message2);

    // Initialize the lock the two condition variables
    pthread_mutex_init(&job_queue_lock, NULL);
    pthread_cond_init(&job_queue_not_full, NULL);
    pthread_cond_init(&job_queue_not_empty, NULL);

    // Wait till threads are complete before main continues. Unless we
    // wait we run the risk of executing an exit which will terminate
    // the process and all threads before the threads have completed.
    pthread_join(producer_thread, NULL);
    pthread_join(executor_thread, NULL);

    printf("command_thread returns: %d\n", iret1);
    printf("executor_thread returns: %d\n", iret2);
    exit(0);
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
        pthread_cond_signal(&job_queue_not_empty); // tell execution process that the buffer isnt empty
        pthread_mutex_unlock(&job_queue_lock); // unlock the job queue
        sleep(ARRIVAL_RATE);
    }

    return 0;
}

struct job remove_from_queue(){
    struct job removed_job = job_queue[job_queue_tail];
    // free(job_queue[job_queue_tail]);
    return removed_job;
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