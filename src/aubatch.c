/*
 *
 * pthread library documentation: 
 * https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
 * 
 * execv(): https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/execv.htm
 *
 */

#include <commandline_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr ); // prints a message
void *commandline(void *ptr); // gets user input

pthread_mutex_t cmd_queue_lock;   // Lock for critical sections
pthread_cond_t cmd_buf_not_full;  // Condition variable for buf_not_full
pthread_cond_t cmd_buf_not_empty; // Condition variable for buf_not_empty

/* Global shared variables */
u_int buf_head;
u_int buf_tail;
u_int jobCount;

int main(){

    // commandline(); // run user interface
    int iret1, iret2; // thread return values
    pthread_t command_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Command Thread";
    char *message2 = "Executor Thread";

    jobCount = 0; // current number of jobs in queue
    buf_head = 0; // next empty space in queue
    buf_tail = 0;
    
    /* Create independent threads each of which will execute function */
    iret1 = pthread_create(&command_thread, NULL, commandline, (void *)message1);
    iret2 = pthread_create(&executor_thread, NULL, print_message_function, (void *)message2);

    /* Initialize the lock the two condition variables */
    pthread_mutex_init(&cmd_queue_lock, NULL);
    pthread_cond_init(&cmd_buf_not_full, NULL);
    pthread_cond_init(&cmd_buf_not_empty, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    pthread_join(command_thread, NULL);
    pthread_join(executor_thread, NULL);

    printf("command_thread returns: %d\n", iret1);
    printf("executor_thread returns: %d\n", iret2);
    exit(0);
}

void *commandline(void *ptr){

	// (1) Send Welcome Message
	printf("Welcome to Caleb Powell's batch job scheduler Version 1.0\n");
	printf("Type 'help' to find more about AUbatch commands.\n");

    char *cmd; //stores the command
    size_t cmdSize = MAXCMDLINE; // max size of command

    // Accept jobs from the user
    while(1){

        // protect queue from concurrent access
        pthread_mutex_lock(&cmd_queue_lock);

        // wait for buffer to not be full
        while (jobCount == BUF_SIZE){
            printf("Buffer Full. Waitting...\n");
            pthread_cond_wait(&cmd_buf_not_full, &cmd_queue_lock);
        }

        // release thread
        pthread_mutex_unlock(&cmd_queue_lock);

        
        printf("\n>"); // Prompt for user input
        
        // (2) Store the user command
        cmd = (char *)malloc(cmdSize * sizeof(char));
        if (cmd == NULL)
        {
            perror("Unable to malloc buffer");
            exit(1);
        }

        getline(&cmd, &cmdSize, stdin); // get user input
        cmd_dispatch(cmd); // evaluate user input
        free(cmd);
        // printf("%d\n", job_buffer[0].time);//run sample_job 10 1
        pthread_cond_signal(&cmd_buf_not_empty); // tell execution process that the buffer isnt empty
        pthread_mutex_unlock(&cmd_queue_lock); // unlock the job queue
    }
	
}

void *print_message_function(void *ptr){
    pthread_cond_wait(&cmd_buf_not_full, &cmd_queue_lock); // wait for signal
    char *message;
    message = (char *)ptr;
    printf("%s \n", message);
	return 0;
}
