#include <iostream>
#include <fstream>
#include <thread.h>


#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

using namespace std;

char** argv_copy;
int argc_copy;
unsigned int numLiveCashiers;
unsigned int boardSize;
unsigned int BOARD_LOCK = 12345;
unsigned int COUT_LOCK = 99999;
unsigned int FULL_CONDITION = 987654321;

int last_num = -1;



void start(void);
void maker_method(void);
void cashier_method(void*);
void initializeBoard();

struct sandwich_order {
	unsigned int cashier;
	int sandwich_num;
	sandwich_order* next;
	sandwich_order* prev;
};

struct board {
	unsigned int full_condition;
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
	unsigned int num;
};

board* myBoard;

int main(int argc, char** argv) {
	argv_copy = argv;
	argc_copy = argc;
	boardSize = atoi(argv[1]);	
	thread_libinit((thread_startfunc_t) start, NULL);
}

void start(void) {
	initializeBoard();
	start_preemptions(false, false, 1);	
	numLiveCashiers = argc_copy-2;
	for (int i = 2; i < argc_copy; i++) {
		cashier* new_cashier = (cashier*) malloc(sizeof(cashier));
		new_cashier->num = i;
		thread_create((thread_startfunc_t) cashier_method, new_cashier);
	}
	thread_create((thread_startfunc_t) maker_method, NULL);
}

void maker_method(void) {
	while (myBoard->curr_size > 0 || numLiveCashiers > 0) {
		if (myBoard->curr_size == 0) {
			thread_yield();
		}
		if (myBoard->curr_size >= myBoard->max_size || myBoard->curr_size >= numLiveCashiers) {
			sandwich_order* sandwich_picked = (sandwich_order*) malloc(sizeof(sandwich_order));
			sandwich_order* curr = (sandwich_order*) malloc(sizeof(sandwich_order));
			int closest_num = 9999;
			sandwich_picked = myBoard->head;
			curr = myBoard->head;
			while (curr != NULL) {
				if (abs(curr->sandwich_num - last_num) < closest_num) {
					sandwich_picked = curr;
					closest_num = abs(curr->sandwich_num - last_num);
				}
				curr = curr->next;
			}
			if (sandwich_picked->next != NULL) {
				sandwich_picked->next->prev = sandwich_picked->prev;
			}
			if (sandwich_picked->prev != NULL) {
				sandwich_picked->prev->next = sandwich_picked->next;
			} else {
				myBoard->head = sandwich_picked->next;
			}
			last_num = sandwich_picked->sandwich_num;
			thread_lock(COUT_LOCK);
			cout << "READY: cashier " << sandwich_picked->cashier-2 << " sandwich " << sandwich_picked->sandwich_num << endl;
			thread_unlock(COUT_LOCK);
			thread_lock(myBoard->lock);
			myBoard->curr_size = myBoard->curr_size - 1;
			thread_unlock(myBoard->lock);
			thread_broadcast(myBoard->lock, myBoard->full_condition);
			thread_signal(myBoard->lock, sandwich_picked->cashier);
		}
		thread_yield();
	}
}

void initializeBoard() { 
    myBoard = (board*) malloc(sizeof(board));
	myBoard->max_size = boardSize;
	myBoard->curr_size = 0;
	myBoard->lock = BOARD_LOCK;
	myBoard->full_condition = FULL_CONDITION;
}

void cashier_method(void* cashier_input) {
	string sandwich;
	cashier* cashier_copy = (cashier*) cashier_input;
	unsigned int cid = cashier_copy->num;
	ifstream myfile (argv_copy[cid]);
  	if (myfile.is_open()) {
  		getline(myfile,sandwich);
  		thread_lock(myBoard->lock);
    	while (myfile) {
    		if (myBoard->curr_size < myBoard->max_size) {
    			thread_lock(COUT_LOCK);
    			cout << "POSTED: cashier " << cid-2 << " sandwich " << sandwich << endl;
      			thread_unlock(COUT_LOCK);

      			sandwich_order* new_sandwich = (sandwich_order*) malloc(sizeof(sandwich_order));
      			new_sandwich->sandwich_num = std::stoi(sandwich);
      			//stoi converts string to integer. Sandwich is read from file as type string!
      			new_sandwich->cashier = cid;
      			new_sandwich->next = myBoard->head;
      			if (myBoard->head != NULL) {
      				myBoard->head->prev = new_sandwich;
      			}
      			myBoard->head = new_sandwich;
      			myBoard->curr_size = myBoard->curr_size + 1;

      			thread_wait(myBoard->lock, cid);
      			getline(myfile,sandwich);
    		} else {
    			thread_wait(myBoard->lock, myBoard->full_condition);
    		}
   		}
   		thread_unlock(myBoard->lock);
   		numLiveCashiers--;
   		myfile.close();
  	} else {
  		printf("Unable to open file for cashier %i\n", cid);
	}
}
