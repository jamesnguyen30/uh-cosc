#include <iostream>
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
	return broadcast -1;
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

int calculate_number_of_hosts(const int subnet){
	int count_0 = count_zero_bits(subnet);
	int number_of_hosts = pow(2, count_0) - 2; 
	return number_of_hosts;
}	

int main()
{

	    string ip_string = "192.168.1.1";
	    string subnet_string = "255.255.255.0";
	    //cout<<ip<<endl;
	    //ip_string = convert_to_string(ip);
	    //cout<<ip_string<<endl;

	    //int no_0_bits = count_zero_bits(ip);
	    //cout<<"Zero bits: "<<no_0_bits<<endl;

	    uint32_t subnet  = convert_to_int(subnet_string);
	    uint32_t ip = convert_to_int(ip_string);

	    cout<<subnet<<endl;
	    subnet_string = convert_to_string(subnet);
	    cout<<"IP "<<ip_string<<endl;
	    subnet_string = convert_to_string(subnet);
	    cout<<"Subnet "<<subnet_string<<endl;
	 
	    uint32_t network = calculate_network(ip, subnet);
	    string network_string = convert_to_string(network);
	    cout<<"Network "<<network_string<<endl;

	    uint32_t broadcast = calculate_broadcast(network, subnet);
	    string broadcast_string = convert_to_string(broadcast);
	    cout<<"broadcast: "<< broadcast_string<<endl;

	    uint32_t min_host = calculate_min_host(network);
	    string min_host_string = convert_to_string(min_host);
	    cout<<"host min: " << min_host_string <<endl; 
	
	    uint32_t max_host = calculate_max_host(broadcast);
	    string max_host_string = convert_to_string(max_host);
	    cout<<"host max: " << max_host_string<<endl; 

	    uint32_t number_of_hosts = calculate_number_of_hosts(subnet);
	    cout<<"hosts " << number_of_hosts<<endl; 

    return 0;
}

