#define MAX_JOB_NAME_LENGTH 10 // maximum length of job name
#define MAX_ARGS 10 // Maximum number of arguments
#define MAX_CMD 64 // max number of char for user input
#define QUEUE_SIZE 10 // size of job queue

#define ARRIVAL_RATE 1 //number of jobs submitted every second
#define SERVICE_RATE 1 //number of jobs submitted every second

extern pthread_mutex_t job_queue_lock;   // Lock for critical sections
extern pthread_cond_t job_queue_not_full;  // Condition variable for not full queue
extern pthread_cond_t job_queue_not_empty; // Condition variable for non empty queue

// Define the Job Queue
struct job {
    char name[MAX_JOB_NAME_LENGTH]; // job name
    int cpu_time;                   // time required to complete the job
    int priority;                   // job priority
    time_t start_time;              // time job was submitted
    time_t finish_time;             // time job finished
    int wait_time;                  // expected wait time
};

struct node {
    struct job job_data;            // data that describes the job
    struct node* next;              // pointer to next job in queue
    struct node* prev;              // pointer to previous job in queue
};


// *** THIS SECTION DEFINES MODULES USED BY THE SCHEDULER THREAD *** //

/*
* Main functionality of the scheduler_thread. Handles the
* job_queue_not_full condition variable, and job_queue_lock
* mutex. Tells the scheduler when to pause execution and
* informs the executor thread that the job_queue in no
* longer empty.
*/
void *scheduler();

/*
* Creates a new job struct using the create_job() function.
* Inserts that new job into the queue using the 
* insert_into_queue() function. Increases the number of jobs in queue
* as well as the number of jobs submitted and then prints information
* about the newly added job
* @param nargs		number of arguments passed to command
* @param args		list of arguments
*/
int add_job(int nargs, char **args);

/*
* Creates a new job object given a list of user arguments.
* Returns the newly created job.
* @param args		list of arguments
*/
struct job create_job(char *args[]);

/*
* Adjusts pointers for the job_list. Handles the job scheduling algorithm.
* Sorts jobs in ascending order based on priority, cpu_time, or start_time
* depending on the policy chosen by the user.
* @param job_data		data to insert into the double linked list
* @param list_head		pointer to the head node in double linked list
*/
int insert_into_queue(struct job job_data, struct node** list_head);

/*
* Changes the policy between fcfs, sjf, and priority.
* @param nargs		number of arguments passed to command
* @param args		list of arguments
*/
int cmd_scheduling(int nargs, char **args);

/*
* Lists information about all the jobs currently in the queue.
* @param nargs		number of arguments passed to command
* @param args		list of arguments
*/
int cmd_list(int nargs, char **args);

/*
* Simulates the user inputting data all at once. Useful if the 
* user does not which to specify what job they would like to execute.
* This function will assume the job executing is named `process`
* and is located in the root directory of the project.
* @param nargs		number of arguments passed to command
* @param args		list of arguments
*/
int automated_input(int nargs, char **args);

// *** THIS SECTION DEFINES MODULES USED BY THE EXECUTOR THREAD *** //

/*
* Main functionality of the executor_thread. Handles the
* job_queue_not_full, and job_queue_not_empty condition variables.
* Handles the job_queue_lock mutex. Tells the executor when to 
* pause execution and informs the scheduler thread that the
* job_queue in no llonger full.
*/
void *executor();

/*
* removed the job from the job_list, frees up memory
* alllocated for the job, and returns iformation about
* the job that was removed.
*/
struct job remove_from_queue();

/*
* forks the executor thread process. Child process uses
* execv(args[0], args) to run the user input while the parent
* process waits for the child process to end before continuing.
* this is the main functionality of the process emulation.
* @param cmd		the command to execute in execv()
*/
void run_cmd(char *cmd);

/*
* Exports the data passed to it into a file within
* the `data` folder as a comma seperated .txt file.
* @param my_job		    data to be stored
* @param my_number		specifies the file name `job_x.txt`
*/
void export_data(struct job my_job, int job_number);

/*
* Reads all data stored within the `data` folder 
* out prints a performance Performance Evaluation.
*/
int show_stats();

/*
* Called by the show_stats() function. Reads an
* individual file and stores its data into a job
* struct.
* @param filename		name of the file where the data is stored
* @param job		    pointer to a job struct where data should be stored
*/
void read_job_from_file(const char* filename, struct job* job);