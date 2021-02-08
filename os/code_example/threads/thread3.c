#include <pthread.h>
#include <stdio.h>

#define NTHREADS 5
#define COUNT 5

struct position
{
	int row;
	int col;
};

void *print_pos(void *pos_void_ptr)
{
	struct position *pos_ptr = (struct position *)pos_void_ptr;
	printf("Position = (%d,%d)\n", pos_ptr->row, pos_ptr->col);
	return NULL;
}

int main()
{
	static struct position pos;
 	pthread_t tid[NTHREADS];
        
	for(int i=0;i<NTHREADS;i++)
	{
		pos.row=i;
		pos.col=i;
		if(pthread_create(&tid[i], NULL, print_pos, &pos)) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}		
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < NTHREADS; i++)
        	pthread_join(tid[i], NULL);
	return 0;
}
