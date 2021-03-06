
#include "../include/connectionHandler.h"
#include "../include/encDec.h"
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
encDec* encoderDecoder;

ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){
	encDec* encoderDecoder = new encDec();
}
    
ConnectionHandler::~ConnectionHandler() {
    close();
    //delete encoderDecoder*;
}
 
bool ConnectionHandler::connect() {
	std::cout << "Starting connect to "
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		cout << "inside" << endl;
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        //encoderDecoder->disconnect=true;
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
        std::cerr << "recv failed 2(Error: " << e.what() << ')' << std::endl;
        //encoderDecoder->disconnect=true;
        return false;
    }
    return true;
}


bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    cout << "sending " << bytesToWrite << "bytes to server!" << endl;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed  3 (Error: " << e.what() << ')' << std::endl;
        //encoderDecoder->disconnect=true;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& bytes) {
	//cout << "getting line " << endl;
	char deli = '/0';
    return getFrameAscii(bytes, deli);
}

bool ConnectionHandler::sendLine(std::string& line) {
	char* ans = this->encoderDecoder->sendFunction(line);
	int packetsize = this->encoderDecoder->Getsizeofpacket();
	cout << "finished sendfunction" << endl;
	if (ans==NULL){
		return true;
	}
	else{
		short OP = this->encoderDecoder->bytesToShort(ans);
		switch (OP){
			case 1: //read request
				cout << "sendLine: read request, packetsize:"<< packetsize <<endl;
				return sendBytes(ans, packetsize);
				break;

			case 2: //write request
				cout << "sendLine: write request, packetsize:"<< packetsize <<endl;
				return sendBytes(ans, packetsize);
				break;

			case 6:
				cout << "sendLine:dirq op, packetsize:"<< packetsize <<endl;
				return sendBytes(ans, packetsize);
				break;
			case 7: // login request
				cout << "sendLine:login op, packetsize:"<< packetsize <<endl;
				for(int i=0;i<packetsize;i++){
					cout << *(ans + i)<<endl;
				}
				cout.flush();
				cout << "check" << endl;
				cout.flush();
				return sendBytes(ans, packetsize);
				break;

			case 8: // delete
				cout << "sendLine: delete request, packetsize:"<< packetsize <<endl;
				return sendBytes(ans, packetsize);
				break;

			case 10: // disc
				cout << "sendLine: disco request, packetsize:"<< packetsize <<endl;
				return sendBytes(ans, packetsize);
				break;
			default:
				cout << "wrong op code from sendfunction" <<endl;
		}
	return true;
	}
}
 
//not in use!!!!
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            if (getBytes(&ch, 1))
            frame.append(1, ch);
            cout << "frame got: " << frame.size() <<endl;
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        //encoderDecoder->disconnect=true;
        return false;
    }
    cout << "bytes got: " << frame.size() <<endl;
    return true;
}

void ConnectionHandler::charToVector (char* source, std::vector<char>* vec){
	for (unsigned int i=0;i<sizeof(source);i++){
		(*vec)[i]=source[i];
	}
}

void ConnectionHandler::vectorToChar(char* tobe, std::vector<char>* source){
	for (unsigned int i=0;i<(*source).size();i++){
		tobe[i]=(*source)[i];
	}
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
       // encoderDecoder->disconnect=true;
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
