#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <sstream>
#define NUM_OCTETS 4
#define NUM_ADDRESS 6

using namespace std;

void error(string msg){
	perror(msg.c_str());
	exit(0);
}

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

struct message{
	int option;
	int account;
	double amount;
};

struct message2{

	int value;
	char buffer[256];
};

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


int8_t get_min_host(uint8_t network, uint8_t & min_host){
	// Returns a carry if there is 
	if(network == 255){
		min_host=0;
		return 1;
	} else {
		min_host =network +  1;
		return 0;
	}
}   

int8_t get_max_host(uint8_t broadcast, uint8_t & max_host){
	// Returns a carry if there is 
	if(broadcast == 0){
		max_host=255;
		return -1;
	} else {
		max_host = broadcast -  1;
		return 0;
	}
}   

int count_zero_bits(const uint8_t n){
	uint8_t count_1 = 0;
	uint8_t temp = n;
	while(temp){
		count_1 += temp & 1;
		temp >>= 1;
	}
	int count_0 = 8 - count_1;
	return count_0;
}

int calculate_number_of_hosts(const uint8_t num_of_zeros){
	int number_of_hosts = pow(2, num_of_zeros) - 2; 
	return number_of_hosts;
}	

void get_network_octet(uint8_t ip, uint8_t subnet, uint8_t &network){
	network = ip & subnet;
}   

void get_broadcast_octet(uint8_t network, uint8_t subnet, uint8_t &broadcast ){
	broadcast =  network | ~subnet; 
}   

void calcualte_network_thread(uint8_t * ip,uint8_t * subnet, uint8_t * network){
	thread threads[NUM_OCTETS];

	for(int i = 0; i< NUM_OCTETS;i++){
		//thread temp = threads[i];
		threads[i] = thread(get_network_octet, ip[i], subnet[i], ref(network[i]));
	}

	for(int i = 0; i< NUM_OCTETS;i++){
		threads[i].join();
	}
}

void calcualte_broadcast_thread(uint8_t * network,uint8_t * subnet, uint8_t * broadcast){
	thread threads[NUM_OCTETS];

	for(int i = 0; i< NUM_OCTETS;i++){
		threads[i] = thread(get_broadcast_octet, network[i], subnet[i], ref(broadcast[i]));
	}

	for(int i = 0; i< NUM_OCTETS;i++){
		threads[i].join();
	}
}

void calculate_min_host(uint8_t * network, uint8_t * min_host){
	uint8_t carry = 1;
	for(int i = NUM_OCTETS - 1; i>=0;i--){
		if(carry==1){
			carry = get_min_host(network[i], min_host[i]);
		} else {
			min_host[i] = network[i];
		}
	}
} 

void calculate_max_host(uint8_t * broadcast, uint8_t * max_host){
	int8_t carry = -1;
	for(int i = NUM_OCTETS - 1; i>=0;i--){
		if(carry==-1){
			carry = get_max_host(broadcast[i], max_host[i]);
		} else {
			max_host[i] = broadcast[i];
		}
	}
} 

response_package start_process(uint8_t ip[NUM_OCTETS], uint8_t subnet[NUM_OCTETS]){
	/*
	 * This function takes a raw string and calculate 
	 * @param
	 * 	uint8_t ip   
	 * 	uint8_t subnet   
	 * @return
	 * 	response_package: calculated addresses
	 * */

	response_package responsePackage;

	uint8_t network_container[NUM_OCTETS];
	uint8_t broadcast_container[NUM_OCTETS];
	uint8_t min_host_container[NUM_OCTETS];
	uint8_t max_host_container[NUM_OCTETS];
	
	calcualte_network_thread(ip, subnet, network_container);
	calcualte_broadcast_thread(network_container, subnet, broadcast_container);
	calculate_min_host(network_container, min_host_container);
	calculate_max_host(broadcast_container, max_host_container);

	for(int i = 0;i<NUM_OCTETS;i++){
		responsePackage.ip[i] = ip[i];
		responsePackage.subnet[i] = subnet[i];
		responsePackage.network[i] = network_container[i];
		responsePackage.broadcast[i] = broadcast_container[i];
		responsePackage.hostmin[i] = min_host_container[i];
		responsePackage.hostmax[i] = max_host_container[i];
	}

	int total_zeros = 0;
	for(int i = 0;i<NUM_OCTETS;i++){
		total_zeros += count_zero_bits(responsePackage.subnet[i]);	
	}

	int num_host = calculate_number_of_hosts(total_zeros); 

	responsePackage.num_host = num_host;

	return responsePackage;
}

int main(int argc, char* argv[]){

	int socketfd, new_socketfd;

	int portno;

	int client_addr_len;

	struct message msg1;

	struct request_package requestPackage;

	//Stores a number of characters read or written
	int n;

	char buffer[256];

	struct sockaddr_in server_addr, client_addr;

	if (argc < 2){
		cout<<"Error! No port provided"<<endl;
		exit(1);	
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd < 0){
		error("Error opening socket");	
	}

	//Clear out the server_addr block	
	bzero((char*) &server_addr, sizeof(server_addr));

	portno = atoi(argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);

	cout<<"SERVER STARTING ... "<<endl;
	cout<<"SERVER NAME "<< portno <<endl;
	cout<<"PORT NUMBER  "<< portno <<endl;

	//Bind the socket to address
	if(::bind(socketfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		error("ERROR on binding");	
	}
	cout<<"STARTED LISTENING FROM CLIENT ... "<<endl;

	listen(socketfd, 5);

	client_addr_len = sizeof(client_addr);

	cout<<"CALLING ACCEPT ... "<<endl;
	//Waiting for connection from client, the process is on hold

	while(1){
	
		new_socketfd = accept(socketfd, (struct sockaddr*) &client_addr, (socklen_t *) &client_addr_len);

		if(new_socketfd < 0){
			error("Error on accept()");	
		}

		pid_t pid = fork();

		if(pid == 0){
			close(socketfd);
		
			cout<<"READING CLIENT WITH CHILD PROCESS ... "<<endl;
			n = read(new_socketfd, &requestPackage, sizeof(struct request_package));

			if(n<0){
				cout<<"Error receiving message"<<endl;	
			} else {
				//debug	
				for(int i = 0;i<NUM_OCTETS;i++){
					cout<<(int)requestPackage.ip[i]<<".";	
				}
			}
			cout<<"READ SUCCESS"<<endl;

			response_package responsePackage;

			responsePackage.network[0] = requestPackage.ip[0] - 100;
			responsePackage.network[1] = requestPackage.ip[1] - 100;
			responsePackage.network[2] = requestPackage.ip[2] - 100;
			responsePackage.network[3] = requestPackage.ip[3] - 100;

			responsePackage = start_process(requestPackage.ip, requestPackage.subnet); 

			n = write(new_socketfd, &responsePackage, sizeof(struct response_package));

			if(n<0){
				cout<<"Error writing message"<<endl;	
			}
			cout<<"WRITE SUCCESS, CLOSING SOCKET AND EXIT CHILD"<<endl;
			exit(0);
		} else {
			cout<<"EXIT PARENT"<<endl;
			close(new_socketfd);
		}
	}	

	return 0;
}
