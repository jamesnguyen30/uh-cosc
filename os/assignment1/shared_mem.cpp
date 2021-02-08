#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

void * create_shared_mem(size_t size){
	
	int protection = PROT_READ | PROT_WRITE;
	int vis = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, size, protection, vis, -1, 0);
}

int main(){
}
