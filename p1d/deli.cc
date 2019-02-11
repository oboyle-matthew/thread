#include <iostream>
#include <fstream>
#include<iostream>
#include<thread>
#include <vector>
#include <pthread.h>
using namespace std;

#define NUM_THREADS 5

char** all_args;

void *Cashier(void *cashierid) {
	string line;
	long cid;
	cid = (long) cashierid;
	printf("Cashier %li is starting\n\n", cid);
	ifstream myfile (all_args[cid]);
  	if (myfile.is_open()) {
  		int line_count = 0;
    	while (getline(myfile,line)) {
    		printf("Cashier = %li \tLine = %i \tText = %s\n", cid, line_count, line.c_str());
      		line_count++;
   		}
   		myfile.close();
  	} else {
  		printf("Unable to open file for cashier %li\n", cid);
	}
	printf("Cashier %li is finished\n\n", cid);
}


int main(int argc, char** argv) {	
	all_args = argv;
    pthread_t threads[NUM_THREADS];
   	int rc;
   	int i;   
   	string line;
   	for( i = 1; i < argc; i++ ) {
      	rc = pthread_create(&threads[i], NULL, Cashier, (void *)i);
      
      	if (rc) {
         	cout << "Error:unable to create thread," << rc << endl;
         	exit(-1);
      	}	
   	}
   	pthread_exit(NULL);
}
