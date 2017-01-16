#include <stdlib.h>
#include "../include/connectionHandler.h"
#include <boost/thread.hpp>
#include <vector>
#include <iostream>
using namespace std;
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
    
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
	
//here we should create the threads and run reader and writer.


    return 0;
}

void reader(ConnectionHandler connectionHandler){
	// We can use one of three options to read data from the server:
	        // 1. Read a fixed number of characters
	        // 2. Read a line (up to the newline character using the getline() buffered reader
	        // 3. Read up to the null character
			std:vector<char> bytes;
	        //std::string answer;
	        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
	        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
	        if (!connectionHandler.getLine(bytes)) {
	            std::cout << "Disconnected. Exiting...\n" << std::endl;
	            break;
	        }
	        len=bytes.size();
			//len=answer.length();
			// A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
			// we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
	        bytes.resize(len-1);
	        //answer.resize(len-1);
	        connectionHandler.encoderDecoder.decode(bytes);

}

void writer(ConnectionHandler connectionHandler){
    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
		int len=line.length();
        if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
    }
}
