#include<iostream>
#include<pthread.h>
#include<unistd.h>


using namespace std;

struct args{
	int row;
	int col;
	int ** mat;
};

void *fill_mat( void * args_void_ptr){

	struct args* arguments = (args*) args_void_ptr; 
	
	int row = arguments->row;
	int col = arguments->col;

	int ** mat = arguments->mat;

	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++){
			mat[i][j] = 10;
		}
	}

	return NULL;
}

void *print_mat( void * args_void_ptr){

	struct args* arguments = (args*) args_void_ptr; 
	
	int row = arguments->row;
	int col = arguments->col;

	int ** mat = arguments->mat;

	for(int i = 0;i<row;i++){
		for(int j = 0;j<col;j++){
			cout<<mat[i][j]<<" ";
		}
		cout<<endl;
	}

	return NULL;
}

void init_mat(int ** mat, int row, int col){
	
	mat = new int*[row];
	for(int i = 0;i<row;i++){
		mat[i] = new int [col];
	}
}

void del_mat(int ** mat, int row, int col){

	for(int i = 0;i<col;i++){
		delete mat[i];
	}

	delete mat;
}

int main(){
	int **a;
	int **b;
	int **c;

	int arow = 10;
	int acol = 20;

	int brow = 10;
	int bcol = 20;
	
	int crow = 10;
	int ccol = 20;

	//a = new int*[arow];	
	//b = new int*[brow];	
	//c = new int*[crow];	

	//init_mat(a, arow, acol);
	//init_mat(b, brow, bcol);
	//init_mat(c, crow, ccol);
	
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;

	struct args* argument1; 
	struct args* argument2; 
	struct args* argument3; 

	argument1->row = arow;
	argument1->col = acol;
	argument1->mat = new int*[arow];
	//for(int i = 0;i<arow;i++){
	//	argument1->mat[i] = new int[acol]; 	
	//}

	//init_mat(argument1->mat, arow, acol);

	//argument2->row = brow;
	//argument2->col = bcol;
	//init_mat(argument2->mat, brow, bcol);

	//argument3->row = crow;
	//argument3->col = ccol;
	//init_mat(argument3->mat, crow, ccol);

	return 0;
	//pthread_create(&tid1, NULL, fill_mat, (void*)argument1);
	//pthread_create(&tid2, NULL, fill_mat, (void*)argument2);
	//pthread_create(&tid3, NULL, fill_mat, (void*)argument3);

	//pthread_join(tid1, NULL);
	//pthread_join(tid2, NULL);
	//pthread_join(tid3, NULL);


	//del_mat(argument1->mat, argument1->row, argument1->col);
	//del_mat(argument2->mat, argument2->row, argument2->col);
	//del_mat(argument3->mat, argument3->row, argument3->col);

	//return 0;

}


