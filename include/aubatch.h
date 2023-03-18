#define MAX_JOB_NAME_LENGTH 10 // maximum length of job name
#define TOTAL_JOB_NUM 5 // total number of jobs to submit 
#define QUEUE_SIZE 10 // size of job queue
#define ARRIVAL_RATE 1 //number of jobs submitted every second
#define SERVICE_RATE 1 //number of jobs submitted every second

void *producer();
void *executor();