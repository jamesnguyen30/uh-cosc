#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	int x =0;
	int i;
	for (i =0; i <2; i++)
		if((pid=fork())==0)  	// child process
			break;
	if (pid == 0 && i ==1)
		fork();
	x++;
	printf("x = %d\n",x);
	sleep(10);
	return 0;
}
