#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <stdio.h> 
#include <string>
#include <sstream>
#include <math.h>
#include <thread>
//#include <pthread.h>

#define NUM_OCTETS 4

using namespace std;

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
	cout<<int(broadcast)<<endl;

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

//struct thread_data{
//	int id;
//	uint8_t a;
//	uint8_t b;
//	uint8_t result;
//};	


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

void start_process(string input_string){
	
	string ip_string = "";
	string subnet_string = "";

	split(input_string, ip_string, subnet_string);

	cout<<"ip "<<ip_string<<endl;
	cout<<"subnet "<<subnet_string<<endl;

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

	for(int i =0;i<NUM_OCTETS;i++){
		cout<< int(network_container[i]) << ".";
	}
	cout<<endl;

	for(int i =0;i<NUM_OCTETS;i++){
		cout<< int(broadcast_container[i]) << ".";
	}
	cout<<endl;
	for(int i =0;i<NUM_OCTETS;i++){
		cout<< int(min_host_container[i]) << ".";
	}
	cout<<endl;
	for(int i =0;i<NUM_OCTETS;i++){
		cout<< int(max_host_container[i]) << ".";
	}
	cout<<endl;

	//Calulate number of hosts 
	//

	int total_zeros = 0;
	for(int i = 0;i<NUM_OCTETS;i++){
		total_zeros += count_zero_bits(subnet_container[i]);	
	}

	int num_hosts = calculate_number_of_hosts(total_zeros); 

	cout<<num_hosts<<endl;
	
	cout<<"=============="<<endl;
}

int main(){
	fstream file;
	file.open("input.txt");
	string line = "";
	while(getline(file,line)){

		int file_d[2];

		pipe(file_d);

		string transfer_data = ""; 

		pid_t pid = fork();

		if(pid != 0 ){
			// pipe[0] read
			// pipe[1] write 

			close(file_d[0]);

			transfer_data = line;
			cout<<"Created child process with id =" << pid << " transfering data to child :"<<transfer_data<<endl;
			int result = write(file_d[1], &transfer_data, sizeof(transfer_data));
			close(file_d[1]);
		}

		if(pid==0) {
			close(file_d[1]);

			string result = read(file_d[0], &transfer_data, sizeof(transfer_data)); 
			cout<< "Child recieved data = " << transfer_data << " from parent id = " << getppid() <<endl;

			//int x = 10;
			//int y = 20;

			//string sample = "192.168.1.1 255.255.255.0";
			//string delim = " ";

			//string ip = sample.substr(0, sample.find(delim));	
			//string subnet = sample.substr(sample.find(delim), sample.size());	

			//cout<<"IP "<<ip<<endl;
			//cout<<"SUBNET "<<subnet<<endl;

			close(file_d[0]);

			exit(1);

		} else if(pid==-1) {
			cout<<"Couldn't create child process"<<endl;
		}
	}
	return 0;
}

