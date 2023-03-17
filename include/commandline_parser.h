#include <stdlib.h>

#define MAXMENUARGS 4 // maximum num of arguments in a command
#define MAXCMDLINE 64 // maximum char size in command
#define MAXJOBNAMELENGTH 10 // maximum length of job name
#define EINVAL 1 // error for invalid input
#define E2BIG 2 // error for too big
#define BUF_SIZE 5 // Size of job queue

struct job {
    char name[MAXJOBNAMELENGTH]; // job name
    int time;       // time required to complete the job
    int priority;   // job priority
};

extern struct job job_buffer[BUF_SIZE];

extern u_int buf_head;
extern u_int buf_tail;
extern u_int jobCount;

extern char policy[10];

int cmd_dispatch(char *cmd);
