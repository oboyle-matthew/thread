#include <iostream>
#include <fstream>
#include <thread.h>



#include <vector>
#include <pthread.h>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

using namespace std;

char** argv_copy;
int argc_copy;
int BOARDSIZE = 2;
int LOCK_NUM = 12345;


void start(void);
void cashier_method(void*);
void initializeBoard();

struct sandwich_order {
	int cashier;
	int sandwich_num;
	sandwich_order* next;
};

struct board {
	unsigned int lock;
	unsigned int curr_size;
	unsigned int max_size; 
	sandwich_order* head;
};

struct test {
	int first;
	int second; 
	int third;
};

struct cashier {
	int num;
};

board* myBoard;

int main(int argc, char** argv) {
	argv_copy = argv;
	argc_copy = argc;	
	thread_libinit((thread_startfunc_t) start, NULL);
}

void start(void) {
	initializeBoard();
	start_preemptions(true, true, 1);	
	cout << "Start\n";
	for (int i = 1; i < argc_copy; i++) {
		cashier* new_cashier = (cashier*) malloc(sizeof(cashier));
		new_cashier->num = i;
		thread_create((thread_startfunc_t) cashier_method, new_cashier);
	}
}

void initializeBoard() {
    myBoard = (board*) malloc(sizeof(board));
	myBoard->max_size = BOARDSIZE;
	myBoard->curr_size = 0;
	myBoard->lock = LOCK_NUM;
}

void cashier_method(void* cashier_input) {
	string line;
	cashier* cashier_copy = (cashier*) cashier_input;
	int cid = cashier_copy->num;
	ifstream myfile (argv_copy[cid]);
  	if (myfile.is_open()) {
  		int line_count = 0;
    	while (getline(myfile,line)) {
    		thread_lock(0);
    		cout << "Cashier = " << cid << "\tLine = " << line_count << "\tText = " << line << "\n";
      		line_count++;
   		}
   		myfile.close();
  	} else {
  		printf("Unable to open file for cashier %i\n", cid);
	}
}
