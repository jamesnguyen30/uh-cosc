#include<iostream>
#include<stdlib.h>
#include<unistd.h>

using namespace std;

int main(){
		
	for(int i = 0;i<3;i++){
		
		pid_t pid = fork();
		if(pid == 0){
			cout<<"This is the child process number "<<i<<endl;	
			if(i == 1){
				pid_t grand_pid = fork();	

				if(grand_pid == 0){
					cout<<"This is the grand child process of 1"<<endl;
					_exit(0);	
				}
				wait(0);
			}	

			_exit(0);
		} else{
			if(i==0){
				cout<<"This is the parent process"<<endl;
			}
		}	
		wait(0);

	}

}
