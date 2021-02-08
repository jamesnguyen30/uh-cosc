#include<pthread.h>
#include<stdio.h>
#include<iostream>
#include<math.h>

#define nthreads 5
#define count 5000

using namespace std;

static pthread_mutex_t bsem;
static pthread_cond_t rcond

void * inc_x(void* arg_void_ptr){
	int *x = (int *) arg_void_ptr;

	for(int i = 0;i<count;i++){
	
		pthread_mutex_lock(&semB);
		(*x)++;
		pthread_mutex_unlock(&semB);
	}
	return NULL;
}
int main(){

	cout<<"Hello world"<<endl;

	static int x = 0;
	pthread_t tid[nthreads];

	cout<<ceil(33/2);

	pthread_mutex_init(&semB, NULL);

	for(int i = 0;i<nthreads;i++){
		if(pthread_create(&tid[i], NULL, inc_x, &x)){
			cout<<"Error creating threads"<<endl;
			return 1;
		}	
	}

	for(int i = 0;i<nthreads;i++){
		pthread_join(tid[i], NULL); 	
	}

	cout<<" Final value of x "<<x<<endl;

	return 0;

}


