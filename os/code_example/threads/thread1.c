#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NTHREADS 5
#define COUNT 5000

void *inc_x(void *x_void_ptr)
{
	int *x_ptr = (int *)x_void_ptr;
//	for (int i=0; i< COUNT; i++)
		(*x_ptr) = (*x_ptr) * 2;
//	sleep(100);
	return NULL;
}

int main()
{
	static int x = 1;
 	pthread_t tid[NTHREADS];
	printf("Initial value of x: %d\n", x);

	for(int i=0;i<NTHREADS;i++)
	{
		if(pthread_create(&tid[i], NULL, inc_x, &x)) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}	
		pthread_join(tid[i], NULL);	
	}
	// Wait for the other threads to finish.
//	for (int i = 0; i < NTHREADS; i++)
  //      	pthread_join(tid[i], NULL);
	printf("Final value of x: %d\n",x);
	return 0;
}
