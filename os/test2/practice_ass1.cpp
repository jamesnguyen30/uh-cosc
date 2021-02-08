#include <math.h>
#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>

using namespace std;

static pthread_mutex_t bsem;
static pthread_cond_t ricon = PTHREAD_COND_INITIALIZER;
static pthread_cond_t castro = PTHREAD_COND_INITIALIZER;
static int turn = 0; //ricon = 0 and castro = 1 

struct arg{
	char *family;
};

void *access_one_at_a_time(void *void_ptr)
{
	
	arg * thread_arg = (arg * ) void_ptr; 
	
	pthread_mutex_lock(&bsem);

	//Wait to see if it's turn for each family member
	if(strcmp(thread_arg->family, "RICON") == 0 && turn != 0){
		//Put thread to sleep
		pthread_cond_wait(&ricon, &bsem);	
	} else if (strcmp(thread_arg->family, "CASTRO") == 0 && turn != 1){
		pthread_cond_wait(&castro, &bsem);	
	}

	//When this thread get the signal, it wakes up and does it stuff
	cout<<thread_arg->family<<" enter "<<endl;

	cout<<thread_arg->family<<" leaving "<<endl;

	//The thread is finish with its stuff, now it signal the other family 

	if(strcmp(thread_arg->family, "RICON") == 0){
		//Put thread to sleep
		turn = 1;
		pthread_cond_signal(&castro);	
	} else if(strcmp(thread_arg->family, "CASTRO") == 0){
		turn = 0;
		pthread_cond_signal(&ricon);	
	}

	pthread_mutex_unlock(&bsem);
	return NULL;
}

int main()
{
	int nmembers;

	std::cin >> nmembers;

	pthread_mutex_init(&bsem, NULL); // Initialize access to 1
	
	pthread_t tid[nmembers];
	arg thread_arg[nmembers];

	//char **family=new char*[nmembers];

	for(int i=0;i<nmembers;i++){

		thread_arg[i].family = new char[10];
		if(i%2==0){
			strcpy(thread_arg[i].family, "CASTRO");	
		} else {
			strcpy(thread_arg[i].family, "RICON");	
		}
	
		if(pthread_create(&tid[i], NULL, access_one_at_a_time, &thread_arg[i])){
			cout<<"Error creating threads"<<endl;
			exit(1);
		}; 

	}

	// Wait for the other threads to finish.
	for (int i = 0; i < nmembers; i++)
		pthread_join(tid[i], NULL);

	//for(int i=0;i<nmembers;i++)
	//	delete[] family[i];

	//delete [] family;
	//delete [] tid;
	return 0;
}
