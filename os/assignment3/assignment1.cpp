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
#include <thread>
#include <sstream>

#define READ_FD 0 
#define WRITE_FD 1 
#define NUM_OCTETS 4
#define NUM_ADDRESS 6

using namespace std;

int share_key  = 100;

const int MAX_STRING_LENGTH = 200;

struct calculated_data{
	uint8_t address[NUM_ADDRESS][NUM_OCTETS];
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

string compile_output(calculated_data data){
	
	string final_string = "";
	string ip = "IP Address: ";
	string subnet = "Subnet: ";
	string network = "Network: ";
	string broadcast = "Broadcast: ";
	string hostmin = "HostMin: ";
	string hostmax = "HostMax: ";

	int i;
	for(i =0;i<NUM_OCTETS-1;i++){
		ip+= 		to_string(data.address[0][i]) + ".";
		subnet+= 	to_string(data.address[1][i]) + ".";
		network+= 	to_string(data.address[2][i]) + ".";
		broadcast+= 	to_string(data.address[3][i]) + ".";
		hostmin+= 	to_string(data.address[4][i]) + ".";
		hostmax+= 	to_string(data.address[5][i]) + ".";
	}

	ip+= 		to_string(data.address[0][i]) + "\n";
	subnet+= 	to_string(data.address[1][i]) + "\n";
	network+= 	to_string(data.address[2][i]) + "\n";
	broadcast+= 	to_string(data.address[3][i]) + "\n";
	hostmin+= 	to_string(data.address[4][i]) + "\n";
	hostmax+= 	to_string(data.address[5][i]) + "\n";

	final_string+= ip;
	final_string+= subnet;
	final_string+= network;
	final_string+= broadcast;
	final_string+= hostmin;
	final_string+= hostmax;

	int total_zeros = 0;
	for(int i = 0;i<NUM_OCTETS;i++){
		total_zeros += count_zero_bits(data.address[1][i]);	
	}

	int num_hosts = calculate_number_of_hosts(total_zeros); 

	string hosts = "# Host: " + to_string(num_hosts);

	final_string+=hosts;

	return final_string;
} 

calculated_data start_process(string input_string){
	/*
	 * This function takes a raw string and calculate 
	 * @param
	 * 	string input_string: ex: 192.168.1.1 255.255.255.0   
	 * */
	string output_string = ""; 

	calculated_data data;
	
	string ip_string = "";
	string subnet_string = "";

	split(input_string, ip_string, subnet_string);

	uint8_t ip_container[NUM_OCTETS];
	uint8_t subnet_container[NUM_OCTETS];
	uint8_t network_container[NUM_OCTETS];
	uint8_t broadcast_container[NUM_OCTETS];
	uint8_t min_host_container[NUM_OCTETS];
	uint8_t max_host_container[NUM_OCTETS];
	
	convert_to_int_array(ip_string, ip_container);
	convert_to_int_array(subnet_string, subnet_container);
	
	calcualte_network_thread(ip_container, subnet_container, network_container);
	calcualte_broadcast_thread(network_container, subnet_container, broadcast_container);
	calculate_min_host(network_container, min_host_container);
	calculate_max_host(broadcast_container, max_host_container);

	for(int i = 0;i<NUM_OCTETS;i++){
		data.address[0][i] = ip_container[i];
		data.address[1][i] = subnet_container[i];
		data.address[2][i] = network_container[i];
		data.address[3][i] = broadcast_container[i];
		data.address[4][i] = min_host_container[i];
		data.address[5][i] = max_host_container[i];
	}
	return data;
}

int main(){

	const int MAX_COUNT = 20;

	string lines[MAX_COUNT];

	key_t key = ftok( "shared_mem", share_key);

	bool initialized_shared_mem = false;

	int COUNT = 0;
	if(initialized_shared_mem == false){
		fstream file;
		file.open("input.txt");
		string line;

		for(COUNT = 0;getline(file,line) && COUNT < MAX_COUNT;COUNT++){
			lines[COUNT] = line;		
		}
		file.close();
	}

	int fd[COUNT][2];
	int pids[COUNT];

	for(int i = 0;i<=COUNT;i++){
		pipe(fd[i]);	
		pids[i] = fork();
		if(pids[i] > 0){
			//shmseg* data = new shmseg(COUNT);
			if(initialized_shared_mem==false){
				initialized_shared_mem = true;
				int shmid = shmget(share_key, sizeof(calculated_data[COUNT]), 0666|IPC_CREAT);

				calculated_data* head; 

				head  = (calculated_data*)shmat(shmid, NULL, 0);

				shmdt(head);
			}

			close(fd[i][READ_FD]);

			int n = write(fd[i][WRITE_FD], &i, sizeof(int));	
			if(n == 0){
				cout<<"There's an error occured in pipe"<<endl;
				return -1;	
			}
			close(fd[i][WRITE_FD]);
			wait(NULL);
		} else if(pids[i]== 0){

			close(fd[i][WRITE_FD]);
			string read_str;	
			int index = 0;
			read(fd[i][READ_FD], &index, sizeof(int));

			int shmid = shmget(share_key, sizeof(calculated_data[COUNT]), 0666|IPC_CREAT);

			calculated_data* head; 

			head  = (calculated_data*)shmat(shmid, NULL, 0);

			calculated_data data = start_process(lines[i]);

			head[index] = data;

			close(fd[i][READ_FD]); 
			shmdt(head);
			_exit(1);
		}
	}

	//Wait for all child to coplete
	for(int i = 0;i< COUNT;i++){
		waitpid(pids[i], NULL, 0);
	}	

	int shmid = shmget(share_key, sizeof(calculated_data[COUNT]), 0666|IPC_CREAT);

	calculated_data * head = (calculated_data*) shmat(shmid, NULL, 0);
	string final_string;
	for(int i = 0;i < COUNT;i++){
		final_string = compile_output(head[i]);
		cout<<final_string<<endl;
		cout<<endl;
	}

	shmdt(head);

	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
