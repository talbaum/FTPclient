
#include "../include/connectionHandler.h"
#include "encDec.cpp"
#include <boost/thread.hpp>
using boost::asio::ip::tcp;
//class encDec;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using boost::thread;
 
bool approve=false;
encDec* encoderDecoder = new encDec;

ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){

}
    
ConnectionHandler::~ConnectionHandler() {
    close();
    //delete encoderDecoder*;
}
 
bool ConnectionHandler::connect() {
	//boost::thread reader(&ConnectionHandler::getFromServer,this); //the thread who reads from the server all the time
	//boost::thread writer(&ConnectionHandler::SendToServer,this); //the thread who waits for user command and acts by it

	std::cout << "Starting connect to "
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
		else{
			//reader.join();
			//writer.join();
		}
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}


bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

void ConnectionHandler::getFromServer(){

}
void ConnectionHandler::SendToServer(){

}
 
bool ConnectionHandler::getLine(std::vector<char> bytes) {
	//std::vector<char> ans;
    //return getFrameAscii(line, '\n');
	//char* bytes[] = new char[512];
	int index=0;
    char ch;
        // Stop when we encounter the null character.
        // Notice that the null character is not appended to the frame string.
        try {
    		do{
    			getBytes(&ch, 1);
    			bytes.push_back(ch);
    			//bytes[index]= ;
                //frame.append(1, ch);
            }while (ch!='\0');
        } catch (std::exception& e) {
            std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
            return false;
        }
        return true;
}

bool ConnectionHandler::sendLine(std::string& line) {
	char* ans = this->encoderDecoder.sendFunction(line);
	short OP = this->encoderDecoder.bytesToShort(ans);

	switch (OP){
	case 1: //read request
		break;

	case 2: //write request
		break;

	case 7: // login request
		return sendBytes(ans, sizeof(ans));
		break;

	case 8: //
		return sendBytes(ans, sizeof(ans));
		break;

	case 10: //
		return sendBytes(ans, sizeof(ans));
		break;
	}

	return true;
}
 
//not in use!!!!
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
		do{
			getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
