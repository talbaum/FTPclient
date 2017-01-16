/*
 * encDec.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: amit
 */

#include "../include/encDec.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
std::string curLine;
int len=0;
int OP;
int ACKblock=-1;
std::string nameOfFile="";
std::vector<char> fileToRead;
int lastblockofdata=0;

encDec::encDec() {
	//this.bytes[] = new char[len];
	OP=0;
}

encDec::~encDec() {
	// TODO Auto-generated destructor stub
}

char* encDec::sendFunction(string& line){
	string command("");
	char* ans = NULL;
	int index=0;
	while ((line.at(index)!=' ')|(line.at(index)!='/n')){
		command=+(line.at(index));
		index++;
	}

	if (command.compare("LOGRQ")){
		ans=CommonPacketWithString(line.substr(index));
		encDec::shortToBytes(7,ans);
	}

	else if (command.compare("DELRQ")){
		ans=CommonPacketWithString(command.substr(index));
		encDec::shortToBytes(8,ans);
	}

	else if (command.compare("RRQ")){
		nameOfFile=command.substr(index,command.size()-1);
		ans=CommonPacketWithString(command.substr(index));
		encDec::shortToBytes(1,ans);
	}

	else if (command.compare("WRQ")){
		ans=CommonPacketWithString(command.substr(index));
		encDec::shortToBytes(2,ans);
	}

	else if (command.compare("DIRQ")){
		//ans=DIRQ(command.substr(index));
		ans= new char[2];
		encDec::shortToBytes(6,ans);

	}

	else if (command.compare("DISC")){
		//ans=DISC(command.substr(index));
		ans= new char[2];
		encDec::shortToBytes(10,ans);
	}

	return ans;
}


char* encDec::CommonPacketWithString(std::string myLine){
	char* ans= NULL;
	if (myLine.size()<1){
		//wrong command error
	}
	else{
		char* packet = new char[2+myLine.size()+1];
		//encDec::shortToBytes(7,packet);
		unsigned int index=2;
		std::string NAME;

		while ((myLine.size()<index) & (myLine.at(index)!=' ')){
			NAME.push_back(myLine.at(index));
			index++;
		}
		if (NAME.size()!=myLine.size()){
			//error - wrong command
		}
		else{
			char* packet2 = encDec::stringToBytes(NAME);
			index=2;
			int index2=0;
			while (index<sizeof(&packet)){
				packet[index]=packet2[index2];
				index++;
				index2++;
			}
		ans = packet;
		}
	}
	return ans;
}


char* encDec::RRQ(std::string myLine){

	return 0;
}

char* encDec::WRQ(std::string myLine){

	return 0;
}

char* encDec::DIRQ(std::string myLine){

	return 0;
}

char* DISC(std::string myLine){

	return 0;
}

char* encDec::stringToBytes(std::string myLine){
	std::vector<char> bytes(myLine.begin(), myLine.end());
	bytes.push_back('0');
	char *c = &bytes[0];
	return c;
}

short encDec::bytesToShort(char* bytesArr)
{
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void encDec::shortToBytes(short num, char* bytesArr)
{
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

char* encDec::decode(std::vector<char> &bytes,ConnectionHandler conHan){
char* bytearr = new char[2];
bytearr[0]=bytes[0];
bytearr[1]=bytes[1];
	this->OP = bytesToShort(bytearr);

	switch (OP){
	case 3:
		handleFileRead(bytes,conHan);
		break;

	case 4:
		bytearr[0]=bytes[2];
		bytearr[1]=bytes[3];
		ACKblock=bytesToShort(bytearr);
		break;

	case 5:
		break;

	case 9:
		break;
	}

return 0;
}

void encDec::handleFileRead(std::vector<char> &bytes,ConnectionHandler conHan){
	char* bytearr = new char[2];
	bytearr[0]=bytes[2];
	bytearr[1]=bytes[3];
	int size = bytesToShort(bytearr);
	bytearr[0]=bytes[4];
	bytearr[1]=bytes[5];
	int block =bytesToShort(bytearr);
	bytes.resize(6,bytes.size());

	if (block==lastblockofdata+1){
		lastblockofdata++;
		fileToRead.insert(fileToRead.end(),bytes.begin(),bytes.end());
		char* approve = makeACK(block);
		conHan.sendBytes(approve,sizeof(approve));
		if (size<512){
			ofstream myFile(nameOfFile, std::ofstream::out | std::ofstream::binary);
			myFile.write(fileToRead.data(),fileToRead.size());

		}
	}
	else{
		cout << "reading file failed" <<endl;
		fileToRead.clear();
		lastblockofdata=0;
		ACKblock=-1;
	}
}

char* encDec::makeACK (int block){
	char* bytearr = new char[4];
	encDec::shortToBytes(4,bytearr);
	char* bytearr2 = new char [2];
	encDec::shortToBytes(block,bytearr2);

	bytearr[2]=bytearr2[0];
	bytearr[3]=bytearr2[1];

	return bytearr;
}



