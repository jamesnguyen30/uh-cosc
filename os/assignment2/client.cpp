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
#include <fstream>
#include <sstream>
#define NUM_OCTETS 4
#define NUM_ADDRESS 6
#define MAX_COUNT 20 

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
	uint8_t ip[NUM_OCTETS];
	uint8_t subnet[NUM_OCTETS];
	uint8_t network[NUM_OCTETS];
	uint8_t broadcast[NUM_OCTETS];
	uint8_t hostmin[NUM_OCTETS];
	uint8_t hostmax[NUM_OCTETS];
	int num_host;
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

void split(const string str, string &ip, string &subnet, char delim = ' '){
	std::stringstream ss(str);
	std::string token;
	int index = 0;
	string ip_subnet[2];
	while (std::getline(ss, token, delim)) {
		ip_subnet[index++]= token ;
	}

	ip = ip_subnet[0];
	subnet = ip_subnet[1];
}

void convert_to_int_array(const std::string& str, uint8_t * container, char delim = '.')
{
	std::stringstream ss(str);
	std::string token;
	int index = 0;
	while (std::getline(ss, token, delim)) {
		int number = stoi(token);
		container[index++]= (uint8_t)number ;
	}
}

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

	if(::connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		error("Error connecting to server");
		exit(0);
	}
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

string compile_output(response_package data){
	
	string final_string = "";
	string ip = "IP Address: ";
	string subnet = "Subnet: ";
	string network = "Network: ";
	string broadcast = "Broadcast: ";
	string hostmin = "HostMin: ";
	string hostmax = "HostMax: ";

	int i;
	for(i =0;i<NUM_OCTETS-1;i++){
		ip+= 		to_string(data.ip[i]) + ".";
		subnet+= 	to_string(data.subnet[i]) + ".";
		network+= 	to_string(data.network[i]) + ".";
		broadcast+= 	to_string(data.broadcast[i]) + ".";
		hostmin+= 	to_string(data.hostmin[i]) + ".";
		hostmax+= 	to_string(data.hostmax[i]) + ".";
	}

	ip+= 		to_string(data.ip[i]) + "\n";
	subnet+= 	to_string(data.subnet[i]) + "\n";
	network+= 	to_string(data.network[i]) + "\n";
	broadcast+= 	to_string(data.broadcast[i]) + "\n";
	hostmin+= 	to_string(data.hostmin[i]) + "\n";
	hostmax+= 	to_string(data.hostmax[i]) + "\n";

	final_string+= ip;
	final_string+= subnet;
	final_string+= network;
	final_string+= broadcast;
	final_string+= hostmin;
	final_string+= hostmax;

	string hosts = "# Host: " + to_string(data.num_host);

	final_string+=hosts;

	return final_string;
} 
int main(int argc, char *argv[]){

	pthread_t tid[MAX_COUNT];

	static struct thread_arg arg[MAX_COUNT];

	string lines[MAX_COUNT];

	if (argc < 3){
		cout<<"Error in arguments"<<endl;	
		exit(0);
	}

	struct hostent* server = gethostbyname(argv[1]);
	int portnum = atoi(argv[2]); 

	int COUNT = 0;

	//fstream file;
	//file.open("input.txt");
	string line;

	while(getline(cin, line) && COUNT < MAX_COUNT){
		lines[COUNT] = line;		
		COUNT++;
	}

	string ip_string =""; 
	string subnet_string =""; 

	for(int i =0;i<COUNT;i++){
		split(lines[i], ip_string, subnet_string); 
		
		convert_to_int_array(ip_string, arg[i].requestPackage.ip);
		convert_to_int_array(subnet_string, arg[i].requestPackage.subnet);


		arg[i].server = server;
		arg[i].portnum = portnum;

		pthread_create(&tid[i], NULL, send_request_to_server, &arg[i]);  
		pthread_join(tid[i], NULL);
	}

	for(int i = 0;i<COUNT;i++){
		cout<< compile_output(arg[i].responsePackage)<< "\n"<< endl;
	}

	return 0;
}
