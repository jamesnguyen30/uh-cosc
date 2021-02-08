#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	int x =0;
	for (int i =0; i <3; i++)
		pid=fork();
	x++;
	printf("x = %d\n",x);
	sleep(10);
	return 0;
}
