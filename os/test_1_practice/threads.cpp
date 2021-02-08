#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

struct args{
	int row;
	int col;
	int **matrix;
};

void *print_matrix(void *args_void_ptr){
	
	cout<<"ARGS LIST"<<endl;
	struct args* arguments = (struct args*) args_void_ptr; 
	
	int row =  arguments->row;
	int col =  arguments->col;

	cout<<row<< " and " << col << endl;
	
	int ** mat = arguments->matrix;

	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++){
			cout<< mat[i][j]<< " ";	
		}	
		cout<<endl;
	}
	return NULL;
}

void init_mat_pointer(int **mat, int row, int col){
	for(int i = 0;i<row;i++){
		mat[i] = new int[col];
	}

	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++){
			mat[i][j] = 1;	
		}
	}
}

int main(){

	cout<<"Hello World"<<endl;
	struct args* arguments;
	int row = 3;
	int col = 5;

	arguments->row = row;
	arguments->col = col;
	arguments->matrix = new int*[row];

	init_mat_pointer(arguments->matrix, row, col);

	pthread_t tid;	
	pthread_create(&tid, NULL, print_matrix, (void*)arguments);
	pthread_join(tid, NULL);

	return 0;
}

