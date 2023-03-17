#define MAXMENUARGS 4 // maximum num of arguments in a command
#define MAXCMDLINE 64 // maximum char size in command
#define EINVAL 1 // error for invalid input
#define E2BIG 2 // error for too big
#define BUF_SIZE 5 // Size of job queue

int cmd_dispatch(char *cmd);
