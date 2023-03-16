#include <stdio.h>
#include <commandline_parser.h>

void *print_message_function(void *ptr){
    char *message;
    message = (char *)ptr;
    printf("%s \n", message);
}

/*
 * Command line main loop.
 */
int commandline()
{
	char *buffer;
	size_t bufsize = 64;

	buffer = (char *)malloc(bufsize * sizeof(char));
	if (buffer == NULL)
	{
		perror("Unable to malloc buffer");
		exit(1);
	}

	while (1)
	{
		printf("> [? for menu]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
	}
	return 0;
}