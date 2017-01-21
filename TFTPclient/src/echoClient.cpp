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
	bool disconnectNow;
public:
	Task (int number, ConnectionHandler* Handler) : _id(number),thisHandler(Handler),disconnectNow(false){	}

	void operator()(){
		if (this->_id==1){
			reader();
		}
		else{
			writer();
		}
	}

	void reader(){

		while (!disconnectNow){ // stay online until?
			std::string answer;
			if (!thisHandler->getLine(answer)) {
				std::cout << "Disconnected. Exiting...\n" << std::endl;
				disconnectNow=true;
				thisHandler->close();
				boost::this_thread::interruption_point();
				break;
			}
			int len=answer.length();
			answer.resize(len-1);
			ConnectionHandler * tmpthis = thisHandler;
			answer =thisHandler->encoderDecoder->decode(answer,tmpthis);
			if ((len>0)&&(answer[0]==-1)){
				disconnectNow=true;//DISCONNECT
			}
		}
	}


	void writer(){
		while (!thisHandler->encoderDecoder->waitForDisc()) {
			const short bufsize = 1024;
			char buf[bufsize];
			std::cin.getline(buf, bufsize);
			std::string line(buf);
			thisHandler->sendLine(line);
			if ((disconnectNow)||(thisHandler->encoderDecoder->wantDisconnect())) {
				std::cout << "Disconnected. Exiting...\n" << std::endl;
				return;
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

	ConnectionHandler thisHandler(host, port);

	if (!thisHandler.connect()) {
		std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
		return 1;
	}
	else{
		cout << "connected!" <<endl;
		Task task1(1,&thisHandler);
		Task task2(2,&thisHandler);

		boost::thread th1(task1);
		boost::thread th2(task2);
		th1.join();
		th2.join();
		th1.interrupt();
		th2.interrupt();
	}
	exit(0);
	return 0;

}
