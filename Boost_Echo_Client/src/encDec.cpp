/*
 * encDec.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: amit
 */

#include "../include/encDec.h"
#include "../include/connectionHandler.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <fstream>
//#include "../include/connectionHandler.h"
using namespace std;

int len=0;
//short OP;
bool expectDir=false;
bool wannaWrite=false;
bool waitForDisconnect=false;
bool disconnect=false;
bool waitForLog=false;
int ACKblock=-1;
string nameOfFile="";
vector<char> fileToRead;
int lastblockofdata=0;
ConnectionHandler* conHan;
int sizeofpacket=0;

encDec::encDec() {
	OP=0;
	conHan = NULL;
}

encDec::~encDec() {
	// TODO Auto-generated destructor stub
}

int encDec::Getsizeofpacket(){
	return sizeofpacket;
}

char* encDec::sendFunction(string& line){
	string command("");
	cout << "sendfuction entered" << endl;
	char* ans = NULL;
	unsigned int index=0;
	while ((line.size()>index)&&(line.at(index)!=' ')){
		command=command+line.at(index);
		index++;
	}

	if ((command=="LOGRQ")&&(line.size()>5)){
		cout << "sendfuction entered LOGRQ " << endl;
		ans=CommonPacketWithString(line.substr(index+1));
		if (ans!=NULL){
		encDec::shortToBytes(7,ans);
		waitForLog=true;
		}
	}

	else if ((command=="DELRQ")&&(line.size()>5)){
		cout << "sendfuction entered DELRQ " << endl;
		ans=CommonPacketWithString(line.substr(index+1));
		encDec::shortToBytes(8,ans);
	}

	else if ((command=="RRQ")&&(line.size()>3)){
		cout << "sendfuction entered RRQ " << endl;
		nameOfFile=line.substr(index+1);
		cout << "after substr" << endl;
		if (nameOfFile.size()>0){
		ans=CommonPacketWithString(nameOfFile);
		encDec::shortToBytes(1,ans);
		}
		else{
			cout << "wrong write command" << endl;
		}
	}

	else if ((command=="WRQ")&&(line.size()>3)){
		cout << "sendfuction entered WRQ " << endl;
		nameOfFile=line.substr(index+1);
		if (nameOfFile.size()>0){
		wannaWrite=true;
		ans=CommonPacketWithString(nameOfFile);
		encDec::shortToBytes(2,ans);
		}
		else{
			cout << "wrong write command" << endl;
		}
	}

	else if (command=="DIRQ"){
		cout << "sendfuction entered DIRQ " << endl;
		ans= new char[2];
		encDec::shortToBytes(6,ans);
		expectDir=true;
		sizeofpacket=2;
	}

	else if (command=="DISC"){
		cout << "sendfuction entered DISC " << endl;
		waitForDisconnect=true;
		ans= new char[2];
		encDec::shortToBytes(10,ans);
		sizeofpacket=2;
	}
	else{
		cout << "you entered a wrong command" << endl;
	}


	return ans;
}


char* encDec::CommonPacketWithString(string myLine){
	char* ans= NULL;

	if (myLine.size()<1){
		cout << "wrong command was entered" <<endl;
	}
	else{
		sizeofpacket = 2+myLine.size()+1;
		///sizeofpacket = 2+myLine.size();
		char* packet = new char[sizeofpacket];
		unsigned int index=0;
		std::string NAME;

		while ((myLine.size()>index) && (myLine.at(index)!=' ')){
			NAME.push_back(myLine.at(index));
			index++;
		}
		if (NAME.size()!=myLine.size()){
			cout << "wrong command was entered" <<endl;
			//error - wrong command
		}
		else{
			char* packet2 = encDec::stringToBytes(NAME);
			index=2;
			unsigned int index2=0;
			while (index2<NAME.length()){
				packet[index]=packet2[index2];
				index++;
				index2++;
			}
			packet[index]='0';
			cout << "packer created is this many bytes: "<< index+1 << endl;

		ans = packet;
		}
	}

	return ans;
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
    //cout << "second line in bytes to short" <<endl;
    result += (short)(bytesArr[1] & 0xff);
    //cout << "3 line in bytes to short" <<endl;
    return result;
}

void encDec::shortToBytes(short num, char* bytesArr)
{
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

char* encDec::decode(std::string& bytes,ConnectionHandler* conHan){
	cout<< "got inside decode function, with string size:" << bytes.size() <<endl;
char* bytearr = new char[2];
	bytearr[0]=bytes.at(0);
	bytearr[1]=bytes.at(1);
	short OP2 = bytesToShort(bytearr);
	cout << "OP found: " << OP2 <<endl;
	switch (OP2){
	case 3:
		if (expectDir){
			handleDIR(bytes);
			expectDir=false;
		}
		else
			handleFileRead(bytes,conHan);
		break;
	case 4:
		bytearr[0]=bytes[2];
		bytearr[1]=bytes[3];
		ACKblock=bytesToShort(bytearr);
		cout << "ACK " << ACKblock << endl;

		if (waitForDisconnect){
			//bytes.clear();
			disconnect=true;
			bytearr[0]=-1;
		}
		else if ((wannaWrite)&(ACKblock==0)){
					handleFileWrite(conHan);
					wannaWrite=false;
				}
		else if (waitForLog){
			cout <<"you are now logged in to the server!"  <<endl;
		}
		break;

	case 5:
		handleError(bytes);
		break;

	case 9:
		handleBroadcast(bytes);
		break;

	default:
		cout <<"something went wrong, maybe bad packet, this is your OP: " << OP2 <<endl;
		break;
	}

return bytearr;
}

void encDec::handleBroadcast(std::string& bytes){
	char delOrAdd = bytes[2];
	int whichOne = delOrAdd;
	string msg1;
	string msg2;
	if (whichOne==1)
		msg1="add";
	else if (whichOne==0)
		msg1="del";
	bytes.resize(3,bytes.size()-1);
	string File(bytes.begin(),bytes.end());

	cout << "BCAST <" << msg1 << "><" << File << ">" << endl;
}

void encDec::handleDIR(std::string& bytes){
	bytes.resize(2,bytes.size()-1);
	string name;
	char end = '\0';
	char cur;
	while (!bytes.empty()){
		cur=bytes.at(0);
		bytes.resize(bytes.size()-1);
		if ((cur==end)&(!name.empty()))
			cout << name << endl;
		else
			name=+cur;
	}
}

void encDec::handleError(std::string& bytes){
	char* bytearr = new char[2];
	bytearr[0]=bytes[2];
	bytearr[1]=bytes[3];
	int errorCode = bytesToShort(bytearr);
	//bytes.resize(6,bytes.size());
	//cout << "msg size is " <<bytes.size() <<endl;

	string ErrorDesc = bytes.substr(4,bytes.size());
	cout << "Error" << errorCode << " - " <<ErrorDesc <<endl;
}
void encDec::handleFileWrite(ConnectionHandler* conHan){
	wannaWrite=false;
	const char* name = nameOfFile.c_str();
	std::ifstream file(name , std::ios::binary);
	file.unsetf(std::ios::skipws);
	std::streampos fileSize;
	file.seekg(0,std::ios::end);
	fileSize=file.tellg();
	file.seekg(0,std::ios::beg);

	vector<char> fileInVector;
	fileInVector.reserve(fileSize);
	fileInVector.insert(fileInVector.begin(), std::istream_iterator<char>(file),std::istream_iterator<char>());

	int block=1;
	vector<char> packet;
	vector<char> curData;
	//int size;
	while (!fileInVector.empty()){
		char* OpIn = new char[2];
		shortToBytes(3,OpIn);
		packet.insert(packet.begin(),OpIn[0],OpIn[1]);
		if (fileInVector.size()>512){
			for (int i=0;i<512;i++){
				char tmp = fileInVector.front();
				fileInVector.erase(fileInVector.begin());
				curData.push_back(tmp);
			}
		}
		else{
			while (!fileInVector.empty()){
				char tmp = fileInVector.front();
				fileInVector.erase(fileInVector.begin());
				curData.push_back(tmp);
			}
		}
		char* sizeIn = new char[2];
		shortToBytes(curData.size(),OpIn);
		packet.insert(packet.end(),sizeIn[0],sizeIn[1]);
		char* blockIn = new char[2];
		shortToBytes(block,OpIn);
		block++;
		packet.insert(packet.end(),blockIn[0],blockIn[1]);
		packet.insert(packet.end(),curData.begin(),curData.end());
		conHan->sendBytes(&packet[0],packet.size());

		while (ACKblock!=block-1){
			//wait!!
		}
		packet.clear();
		curData.clear();
	}
}

void encDec::handleFileRead(std::string& bytes,ConnectionHandler* conHan){
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
		conHan->sendBytes(approve,sizeof(approve));
		if (size<512){
			const char* name = nameOfFile.c_str();
			ofstream myFile;
			myFile.open(name, std::ofstream::out | ofstream::binary);
			myFile.write(fileToRead.data(),fileToRead.size());
			fileToRead.clear();
			lastblockofdata=0;
			ACKblock=-1;
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

bool encDec::wantDisconnect(){
	return disconnect;
}



