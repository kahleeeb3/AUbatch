#define MAXMENUARGS 4
#define MAXCMDLINE 64
#define EINVAL 1
#define E2BIG 2

void commandline();
int cmd_dispatch(char *cmd);
