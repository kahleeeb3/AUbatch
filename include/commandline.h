void get_input();
int parsecmd(char *cmd, char *args[], int *num_args);
int cmd_dispatch(char *args[], int num_args);
void showmenu(const char *name, const char *x[]);
int cmd_helpmenu(int n, char **a);
int cmd_quit(int nargs, char **args);