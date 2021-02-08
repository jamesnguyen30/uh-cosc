#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <stdio.h> 
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

void split(const std::string& str, unsigned int * container, char delim = '.')
{
    std::stringstream ss(str);
    std::string token;
    int index = 0;
    while (std::getline(ss, token, delim)) {
        int number = stoi(token);
        container[index++]=number;
    }
}

uint32_t convert_to_int(const std::string& str, char delim = '.')
{
    std::stringstream ss(str);
    std::string token;
    int index = 0;
    uint32_t container[4];
    while (std::getline(ss, token, delim)) {
        int number = stoi(token);
        container[index++]=number;
    }
    
    uint32_t value = (container[0] << 24) + (container[1] << 16) + (container[2] << 8) + container[3]; 

    return value;
}

string convert_to_string(const uint address){
	string a = "";
	string oct = to_string((address & 0xff000000) >> 24);
	a.append(oct + ".");
	oct = to_string((address & 0x00ff0000) >> 16);
	a.append(oct + ".");
	oct = to_string((address & 0x0000ff00) >> 8);
	a.append(oct + ".");
	oct = to_string((address & 0x000000ff));
	a.append(oct);

	return a;

}

void print_ip(const unsigned int * octet){
    for(int i = 0;i<4;i++){
        cout<< octet[i]<<".";
    }
    cout<<endl;
    
}

int calculate_network(const int ip, const int subnet){
	return ip & subnet;
}

int calculate_broadcast(const int network, const int subnet){
	return network | ~subnet;
}

int calculate_min_host(const int network){
	return network +1;
}

int calculate_max_host(const int broadcast){
	return broadcast +1;
}

int count_zero_bits(const int n){
	int count_1 = 0;
	unsigned int temp = n;
	while(temp){
		count_1 += temp & 1;
		temp >>= 1;
	}
	int count_0 = 32 - count_1;
	return count_0;
}

int main(){
	int data[6] = {1,2,3,4,5,6};


	int transfer_data = 0;
	//file.open("input.txt");
	//string line = "";
	//while(getline(file,line)){
	//
	//	cout<<line<<endl;
	//}

	for(int i = 0;i<6;i++){
		int file_d[2];
		pipe(file_d);
		pid_t pid = fork();
		if(pid != 0 ){
			// pipe[0] read
			// pipe[1] write 

			close(file_d[0]);

			transfer_data = data[i];
			cout<<"Created child process with id =" << pid << " transfering data to child :"<<transfer_data<<endl;
			int result = write(file_d[1], &transfer_data, sizeof(transfer_data));
			close(file_d[1]);
		}

		if(pid==0) {
			close(file_d[1]);

			int result = read(file_d[0], &transfer_data, sizeof(transfer_data)); 
			cout<< "Child recieved data = " << transfer_data << " from parent id = " << getppid() <<endl;

			int x = 10;
			int y = 20;

			string sample = "192.168.1.1 255.255.255.0";
			string delim = " ";

			string ip = sample.substr(0, sample.find(delim));	
			string subnet = sample.substr(sample.find(delim), sample.size());	

			cout<<"IP "<<ip<<endl;
			cout<<"SUBNET "<<subnet<<endl;

			close(file_d[0]);

			// thread obj(callback,ref(x),"Ahaha");
			// thread obj2(callback,ref(y),"Love Love");
			// thread obj2(callback,ref(y),"Love Love");

			exit(1);

		} else if(pid==-1) {
			cout<<"Couldn't create child process"<<endl;
		}
	}
	return 0;

