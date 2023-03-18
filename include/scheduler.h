#define MAX_JOB_NAME_LENGTH 10 // maximum length of job name
#define TOTAL_JOB_NUM 5 // total number of jobs to submit 
#define QUEUE_SIZE 10 // size of job queue
#define ARRIVAL_RATE 1 //number of jobs submitted every second
#define SERVICE_RATE 1 //number of jobs submitted every second

extern pthread_mutex_t job_queue_lock;   // Lock for critical sections
extern pthread_cond_t job_queue_not_full;  // Condition variable for not full queue
extern pthread_cond_t job_queue_not_empty; // Condition variable for non empty queue

void *producer();
void *executor();
void print_job_queue();
struct job create_job();
void insert_into_queue(struct job *job_ptr);
void *producer();
struct job remove_from_queue();
void *executor();