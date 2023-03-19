#define MAX_JOB_NAME_LENGTH 10 // maximum length of job name
#define MAX_ARGS 10 // Maximum number of arguments
#define MAX_CMD 64 // max number of char for user input

// #define TOTAL_JOB_NUM 5 // total number of jobs to submit 
// #define QUEUE_SIZE 10 // size of job queue
#define TOTAL_JOB_NUM 5 // total number of jobs to submit 
#define QUEUE_SIZE 3 // size of job queue

#define ARRIVAL_RATE 1 //number of jobs submitted every second
#define SERVICE_RATE 1 //number of jobs submitted every second

extern pthread_mutex_t job_queue_lock;   // Lock for critical sections
extern pthread_cond_t job_queue_not_full;  // Condition variable for not full queue
extern pthread_cond_t job_queue_not_empty; // Condition variable for non empty queue

void *scheduler();
int add_job(int nargs, char **args);
int cmd_list(int nargs, char **args);
// int automated_input(int nargs, char **args);
int cmd_scheduling(int nargs, char **args);

void *executor();
void run_cmd(char *cmd);
struct job remove_from_queue();