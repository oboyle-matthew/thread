#include <iostream>
#include <fstream>
#include<iostream>
#include<thread.h>
using namespace std;

void start(void);
void printFiles(void);

int main(int argc, char** argv) {
	thread_libinit((thread_startfunc_t) start, NULL);
}

void start(void){
	cout << "In function start, going to create a new thread\n";
	thread_create((thread_startfunc_t) printFiles, NULL);
}

void printFiles(void){
	//cout << "You have entered " << argc-1 << " arguments.\n";
	cout << "Hello World! In the printFiles function\n";
	/*
	string line;
	for (int i = 1; i < argc; i++) {
		cout << "Attempting to read file " << argv[i] << ":\n";
		ifstream myfile (argv[i]);
	  	if (myfile.is_open()) {
	    		while (getline(myfile,line)) {
	      			cout << line << '\n';
	   		 }
	   		 myfile.close();
	  	} else {
			cout << "Unable to open file\n";
		}
		cout << "\n\n";
	}
	*/
}

