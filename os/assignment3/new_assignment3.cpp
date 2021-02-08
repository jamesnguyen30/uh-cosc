#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sstream>
#include <math.h>

#define NUM_OCTETS 4

using namespace std;

static pthread_mutex_t mutex_child;
static pthread_mutex_t mutex_grandchild[100];
static pthread_mutex_t mutex_child_wait_grandchild[100];

static pthread_cond_t cond_child[100];
static pthread_cond_t cond_grandchild[100];

static int order_static = 0;

struct child_arg{
	string ip_and_subnet;
	int order;
};

struct grandchild_arg{
	uint8_t ip[NUM_OCTETS];
	uint8_t subnet[NUM_OCTETS];
	uint8_t network[NUM_OCTETS];
	uint8_t broadcast[NUM_OCTETS];
	uint8_t min_host[NUM_OCTETS];
	uint8_t max_host[NUM_OCTETS];
	int num_host;
	int child_thread_num;
	bool is_network_available = false;
};

string compile_output(grandchild_arg data){
	
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
		hostmin+= 	to_string(data.min_host[i]) + ".";
		hostmax+= 	to_string(data.max_host[i]) + ".";
	}

	ip+= 		to_string(data.ip[i]) + "\n";
	subnet+= 	to_string(data.subnet[i]) + "\n";
	network+= 	to_string(data.network[i]) + "\n";
	broadcast+= 	to_string(data.broadcast[i]) + "\n";
	hostmin+= 	to_string(data.min_host[i]) + "\n";
	hostmax+= 	to_string(data.max_host[i]) + "\n";

	final_string+= ip;
	final_string+= subnet;
	final_string+= network;
	final_string+= broadcast;
	final_string+= hostmin;
	final_string+= hostmax;

	string hosts = "# Hosts: " + to_string(data.num_host);

	final_string+=hosts;

	return final_string;
} 

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

void print_address_to_string(uint8_t *address){
	for(int i = 0;i<4;i++){
		cout<<(int)*(address + i)<<".";	
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

void get_network_octet(uint8_t* ip, uint8_t* subnet, uint8_t *network){
	for(int i =0;i<NUM_OCTETS;i++){
		network[i] = ip[i] & subnet[i];	
	}
}   

void get_broadcast_octet(uint8_t* network, uint8_t* subnet, uint8_t *broadcast ){
	for(int i =0;i<NUM_OCTETS;i++){
		broadcast[i] =  network[i] | ~subnet[i];
	}
}   

void get_min_host(uint8_t * network, uint8_t * min_host){
	uint8_t carry = 1;
	for(int i = NUM_OCTETS - 1; i>=0;i--){
		if(carry==1){
			carry = get_min_host(network[i], min_host[i]);
		} else {
			min_host[i] = network[i];
		}
	}
} 

void get_max_host(uint8_t * broadcast, uint8_t * max_host){
	int8_t carry = -1;
	for(int i = NUM_OCTETS - 1; i>=0;i--){
		if(carry==-1){
			carry = get_max_host(broadcast[i], max_host[i]);
		} else {
			max_host[i] = broadcast[i];
		}
	}
} 

void * grandchild_print_message(void * arg_void_ptr){

	struct grandchild_arg* argument = (grandchild_arg*) arg_void_ptr;
	int child_thread_num = argument->child_thread_num;
	//cout<<"Child thread num " << child_thread_num << endl;
	pthread_mutex_lock(&mutex_grandchild[child_thread_num]);
	bool* is_network_available =&(argument->is_network_available);

	//CRITICAL SECTION
	if(*is_network_available == false){
		get_network_octet(argument->ip, argument->subnet, argument->network);
		get_min_host(argument->network, argument->min_host);
		*is_network_available = true;
	} else {
		get_broadcast_octet(argument->network, argument->subnet, argument->broadcast);
		get_max_host(argument->broadcast, argument->max_host);
		pthread_cond_signal(&cond_grandchild[child_thread_num]);
	}

	pthread_mutex_unlock(&mutex_grandchild[child_thread_num]);

	pthread_exit(NULL);
}

void * print_message(void * arg_void_ptr){

	pthread_mutex_lock(&mutex_child);

	struct child_arg* argument = (child_arg*) arg_void_ptr;		

	int order = argument->order;

	pthread_t g_tid[2]; 

	grandchild_arg grandchild_args;
	grandchild_args.child_thread_num = order;

	string ip_and_subnet = argument->ip_and_subnet;	

	string ip;
	string subnet;

	split(ip_and_subnet, ip, subnet);	

	convert_to_int_array(ip, grandchild_args.ip);
	convert_to_int_array(subnet, grandchild_args.subnet);



	if(pthread_create(&g_tid[0], NULL, grandchild_print_message, (void*)&grandchild_args)){
		cout<<"Error creating pthread for network calculation"<<endl;
		pthread_exit(NULL);
	}
	if(pthread_create(&g_tid[1], NULL, grandchild_print_message, (void*)&grandchild_args)){
		cout<<"Error creating pthread for broadcast calculation"<<endl;
		pthread_exit(NULL);
	}

	int total_zeros = 0;
	for(int i =0;i<NUM_OCTETS;i++){
		total_zeros += count_zero_bits(grandchild_args.subnet[i]);	
	}

	int num_host = calculate_number_of_hosts(total_zeros);

	grandchild_args.num_host = num_host;

	pthread_cond_wait(&cond_grandchild[order], &mutex_child_wait_grandchild[order]);

	if(order != order_static){
		pthread_cond_wait(&cond_child[order], &mutex_child);	
	}

	string final_str = compile_output(grandchild_args);	

	cout<<final_str<<endl<<endl;

	order_static++;

	pthread_cond_signal(&cond_child[order_static]);
	
	pthread_mutex_unlock(&mutex_child);

	pthread_join(g_tid[0], NULL);
	pthread_join(g_tid[1], NULL);


	pthread_exit(NULL);
}

int main(){
	const int MAX_COUNT = 20;
	string lines[MAX_COUNT];

	int COUNT = 0;
	string line;
    
    getline(cin,line);
	while(getline(cin, line) && COUNT < MAX_COUNT){
		lines[COUNT] = line;		
		COUNT++;
	}

	pthread_mutex_init(&mutex_child, NULL);
	for(int i = 0;i<COUNT;i++){
		pthread_mutex_init(&mutex_grandchild[i], NULL);
		pthread_mutex_init(&mutex_child_wait_grandchild[i], NULL);
	}

	pthread_t tid[COUNT];
	child_arg arguments[COUNT];

	for(int i = 0;i<COUNT;i++){
		arguments[i].ip_and_subnet = lines[i];
		arguments[i].order = i;
		cond_child[i] = PTHREAD_COND_INITIALIZER;

		cond_grandchild[i] = PTHREAD_COND_INITIALIZER; 

		if(pthread_create(&tid[i], NULL, print_message, (void*)&arguments[i])){
			cout<<"Error creating threads"<<endl;	
			return 1;
		}
	}

	for(int i = 0;i<COUNT;i++){
		pthread_join(tid[i], NULL);
	}

	pthread_exit(NULL);
	return 0;

}

