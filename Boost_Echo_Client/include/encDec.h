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
	short OP;
	std::string CurLine;
	ConnectionHandler* conHan;

public:
	encDec();
	virtual ~encDec();
	//char* encode(const std::string& s);
	char* decode(std::string& bytes1,ConnectionHandler* conHan);
	short getOp();
	char* sendFunction (std::string& s); //get a line from the user and do the right action based on it and gets the packet that need to send to the server.
	short bytesToShort(char* bytesArr); //takes 2 bytes and make them a short (for OP and block)
	void shortToBytes(short num, char* bytesArr); //takes a short and make it bytes to send to server.
	char* stringToBytes(std::string myLine); //takes a string and convert to byte array.
	char* CommonPacketWithString(short opCpde, std::string myLine);
	void handleFileRead(std::string& bytes1,ConnectionHandler* conHan);
	void handleFileWrite(ConnectionHandler* conHan);
	void handleError(std::string& bytes1);
	void handleBroadcast(std::string& bytes1);
	void handleDIR(std::string& bytes1);
	char* makeACK (int block);
	int Getsizeofpacket();
	bool wantDisconnect();
	bool waitForDisc();
	bool disconnect=false;
};

#endif /* SRC_ENCDEC_H_ */
