// https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
// execv(): https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/execv.htm

// #include <commandline_parser.h>
#include <stdio.h> // printf()
#include <stdlib.h> // srand(), exit()
#include <pthread.h> // threads
#include <scheduler.h>

pthread_mutex_t job_queue_lock;   // Lock for critical sections
pthread_cond_t job_queue_not_full;  // Condition variable for not full queue
pthread_cond_t job_queue_not_empty; // Condition variable for non empty queue

int main(){
    printf("\n");
    srand(time(NULL)); // seed the random number generator with the current time
    // commandline(); // run user interface
    int iret1, iret2; // thread return values
    pthread_t producer_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Producer Thread";
    char *message2 = "Executor Thread";
    
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