#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#define NUM_OCTETS 4
#define NUM_ADDRESS 6

using namespace std;

void error(string message){

	perror(message.c_str());
	exit(0);
};

struct request_package{
	uint8_t ip[NUM_OCTETS];
	uint8_t subnet[NUM_OCTETS];
};

struct response_package{
	uint8_t network[NUM_OCTETS];
	uint8_t broadcast[NUM_OCTETS];
	uint8_t hostmin[NUM_OCTETS];
	uint8_t hostmax[NUM_OCTETS];
};

struct thread_arg{
	request_package requestPackage;
	response_package responsePackage;
	hostent *server;
	int portnum;
};

struct message{
	int option;
	int account;
	double amount;
};

struct message2{
	int value;
	char buffer[256];
};

void *send_request_to_server(void* arg_void_ptr){
	
	struct thread_arg *arg_ptr = (struct thread_arg *)arg_void_ptr;
	int portnum = arg_ptr->portnum;
	struct hostent *server = arg_ptr->server;
	int socketfd, n;
	struct sockaddr_in server_addr;
	struct request_package requestPackage;
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd < 0){
		error("Error opening socket");
		exit(0);	
	}

	if(server == NULL){
		cout<<"HOST not found"<<endl;	
		exit(0);
	}

	bzero((char*) &server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	bcopy((char*) server->h_addr, (char*) &server_addr.sin_addr.s_addr, server->h_length);

	server_addr.sin_port = htons(portnum);

	cout<<"CONNECTING TO SERVER "<<endl;

	if(::connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		error("Error connecting to server");
		exit(0);
	}

	cout<<"Size of data package "<<sizeof(struct request_package)<<endl;

	//bzero((request_package*) &arg_ptr, sizeof(request_package)); 

	n = write(socketfd, &arg_ptr->requestPackage, sizeof(struct request_package));

	if(n<0){
		error("ERROR writing to socket");	
	}

	n = read(socketfd, &arg_ptr->responsePackage, sizeof(struct response_package));

	if(n<0){
		error("ERROR reading to socket");	
	}

	close(socketfd);
	return NULL;
}

int main(int argc, char *argv[]){

	pthread_t tid[20];

	static struct thread_arg arg[20];

	if (argc < 3){
		cout<<"Error in arguments"<<endl;	
		exit(0);
	}

	struct hostent* server = gethostbyname(argv[1]);
	int portnum = atoi(argv[2]); 

	arg[0].server = server;
	arg[0].portnum = portnum;

	arg[0].requestPackage.ip[0] =192;
	arg[0].requestPackage.ip[1] =168;
	arg[0].requestPackage.ip[2] =1;
	arg[0].requestPackage.ip[3] =1;

	arg[0].requestPackage.subnet[0] =255;
	arg[0].requestPackage.subnet[1] =255;
	arg[0].requestPackage.subnet[2] =255;
	arg[0].requestPackage.subnet[3] =0;


	arg[1].server = server;
	arg[1].portnum = portnum;

	arg[1].requestPackage.ip[0] =192;
	arg[1].requestPackage.ip[1] =168;
	arg[1].requestPackage.ip[2] =100;
	arg[1].requestPackage.ip[3] =24;

	arg[1].requestPackage.subnet[0] =255;
	arg[1].requestPackage.subnet[1] =255;
	arg[1].requestPackage.subnet[2] =255;
	arg[1].requestPackage.subnet[3] =248;

	arg[2].server = server;
	arg[2].portnum = portnum;

	arg[2].requestPackage.ip[0] =172;
	arg[2].requestPackage.ip[1] =17;
	arg[2].requestPackage.ip[2] =0;
	arg[2].requestPackage.ip[3] =1;

	arg[2].requestPackage.subnet[0] =255;
	arg[2].requestPackage.subnet[1] =255;
	arg[2].requestPackage.subnet[2] =255;
	arg[2].requestPackage.subnet[3] =240;

	pthread_create(&tid[0], NULL, send_request_to_server, &arg[0]);  
	pthread_create(&tid[1], NULL, send_request_to_server, &arg[1]);  
	pthread_create(&tid[2], NULL, send_request_to_server, &arg[2]);  

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	cout<<"Request"<<endl;
	for(int i = 0;i<NUM_OCTETS;i++){
		cout<<(int)arg[0].requestPackage.ip[i]<<".";	
	}
	cout<<endl;
	cout<<"Response"<<endl;
	for(int i = 0;i<NUM_OCTETS;i++){
		cout<<(int)arg[0].responsePackage.ip[i]<<".";	
	}

	return 0;
}

