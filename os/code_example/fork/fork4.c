#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	pid_t pid;
	char *cmd = "ps";
	char *argv[3];
	argv[0] = "ps";
	argv[1] = "-all";
	argv[2] = NULL;
	if ((pid=fork())==0)
		if (execvp(cmd, argv) < 0) 
		{     
                	printf("*** ERROR: exec failed\n");
                	exit(1);
            	}
	wait(0);
	printf("Parent process ends\n");
	return 0;
}
