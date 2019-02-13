#include <iostream>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <pthread.h>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
using namespace std;

char** all_args;
int board_size;
std::list<int> deli_list;

struct sandwich {
	int cashier_id;
	int num;
};

void *Cashier(void *cashierid) {
	string line;
	long cid;
	cid = (long) cashierid;
	printf("Cashier %li is starting \n\n", cid);
	ifstream myfile (all_args[cid]);
  	if (myfile.is_open()) {
  		int line_count = 0;
    	while (getline(myfile,line)) {
    		if (line_count == 0) {
    			int sandwich_num = std::stoi(line);
    			printf("Cashier = %li \tLine = %i \tSandwich number =  %li\n", cid, line_count, sandwich_num);
    			deli_list.push_back(sandwich_num);
    		}
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
	board_size = atoi(argv[1]);
	cout << board_size;
	all_args = argv;
    pthread_t cashiers[argc];
    pthread_t makers[1];
   	int rc;
   	int i;   
   	string line;
   	for( i = 2; i < argc; i++ ) {
      	rc = pthread_create(&cashiers[i], NULL, Cashier, (void *)i);
      
      	if (rc) {
         	cout << "Error:unable to create thread," << rc << endl;
         	exit(-1);
      	}	
   	}
   	sleep(1);
   	int index = 0;
   	cout << "\n\n\nHere is the current sandwich list:\n";
   	for (int x : deli_list) {
		cout << x;
		cout << "\n";
	}
   	pthread_exit(NULL);
   	
}
