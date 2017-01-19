#include <stdlib.h>
#include "../include/connectionHandler.h"
#include "../include/encDec.h"
#include <boost/thread.hpp>
#include <vector>
#include <iostream>
using namespace std;


class Task{
private:
	int _id;
	ConnectionHandler* thisHandler;

public:
	Task (int number, ConnectionHandler* Handler) : _id(number),thisHandler(Handler){	}

	void operator()(){
		if (this->_id==1){
			reader();
		}
		else{
			writer();
		}
	}

	void reader(){

				vector<char> bytes;
				while (1){ // stay online until?
					if (!thisHandler->getLine(bytes)) {
						std::cout << "Disconnected. Exiting...\n" << std::endl;
						return;
					}
					bytes.resize(bytes.size()-1);
					ConnectionHandler * tmpthis = thisHandler;
					thisHandler->encoderDecoder->decode(bytes,tmpthis);
					if (bytes[0]==-1){
						//DISCONNECT
					}
				}

	}

	void writer(){
	    while (1) {
	        const short bufsize = 1024;
	        char buf[bufsize];
	        std::cin.getline(buf, bufsize);
			std::string line(buf);
			//int len=line.length();
	        if (!thisHandler->sendLine(line)) {
	            std::cout << "Disconnected. Exiting...\n" << std::endl;
	            break;
	        }
	    }
	}
};
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {
    if (argc < 3) {
         std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    

    //ConnectionHandler thisHandler(host, port);
    char* thisip = "132.73.194.21"; //me
    //char* thisip = "132.73.204.16";

    		short thisport = atoi("8888");

    ConnectionHandler thisHandler(thisip, thisport);

    if (!thisHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
    else{
    	cout << "connected!" <<endl;
    	boost::mutex mutex;
    	    Task task1(1,&thisHandler);
    	    Task task2(2,&thisHandler);

    	    boost::thread th1(task1);
    	    boost::thread th2(task2);
    	    th1.join();
    	    th2.join();
    }

//here we should create the threads and run reader and writer.


    return 0;
}

