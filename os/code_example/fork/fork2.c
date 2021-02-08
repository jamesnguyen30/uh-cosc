#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	int x =0;
	for (int i =0; i <3; i++)
		if((pid=fork())==0)  	// child process
			break;
	if (pid!=0)	// parent process
		for (int i=0;i<3;i++)
			wait(NULL);
	x++;
	printf("x = %d\n",x);
	return 0;
}
