/*
 * encDec.h
 *
 *  Created on: Jan 15, 2017
 *      Author: amit
 */
#include <string>
#include <vector>
//#include "../include/connectionHandler.h"
#ifndef SRC_ENCDEC_H_
#define SRC_ENCDEC_H_
using namespace std;
class ConnectionHandler;

class encDec {
private:
	char bytes[];
	int OP;
	std::string CurLine;
	ConnectionHandler* conHan;

public:
	encDec();
	virtual ~encDec();
	//char* encode(const std::string& s);
	char* decode(std::vector<char>& bytes1,ConnectionHandler* conHan);
	short getOp();
	char* sendFunction (std::string& s); //get a line from the user and do the right action based on it and gets the packet that need to send to the server.
	short bytesToShort(char* bytesArr); //takes 2 bytes and make them a short (for OP and block)
	void shortToBytes(short num, char* bytesArr); //takes a short and make it bytes to send to server.
	char* stringToBytes(std::string myLine); //takes a string and convert to byte array.
	char* CommonPacketWithString(std::string myLine);
	void handleFileRead(std::vector<char>& bytes1,ConnectionHandler* conHan);
	void handleFileWrite(ConnectionHandler* conHan);
	void handleError(std::vector<char>& bytes1);
	void handleBroadcast(std::vector<char>& bytes1);
	void handleDIR(std::vector<char>& bytes1);
	char* makeACK (int block);
	int Getsizeofpacket();
	bool wantDisconnect();
};

#endif /* SRC_ENCDEC_H_ */
