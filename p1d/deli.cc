#include <iostream>
#include <fstream>
#include<iostream>
using namespace std;

int main(int argc, char** argv) {
	cout << "You have entered " << argc-1 << " arguments.\n";
	cout << "Hello World!\n";
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
}
