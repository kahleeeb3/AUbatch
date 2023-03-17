/*
 *
 * https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
 * https://man7.org/linux/man-pages/man3/pthread_create.3.html
 *
 */

#include <commandline_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function(void *ptr){
    char *message;
    message = (char *)ptr;
    printf("%s \n", message);
	return 0;
}

int main(){

    // commandline(); // run user interface

    pthread_t command_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Command Thread";
    char *message2 = "Executor Thread";
    int iret1, iret2;

    /* Create independent threads each of which will execute function */

    iret1 = pthread_create(&command_thread, NULL, print_message_function, (void *)message1);
    iret2 = pthread_create(&executor_thread, NULL, print_message_function, (void *)message2);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);

    // printf("Thread 1 returns: %d\n", iret1);
    // printf("Thread 2 returns: %d\n", iret2);
    exit(0);
}