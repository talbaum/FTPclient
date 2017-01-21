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
using namespace std;

int len=0;
//short OP;
bool expectDir=false;
bool wannaWrite=false;
bool waitForDisconnect=false;
bool disconnect=false;
bool waitForLog=false;
int finishWrite=0;
int ACKblock=-1;
string nameOfFile="";
vector<char> fileToRead;
int lastblockofdata=0;
ConnectionHandler* conHan;
int sizeofpacket=0;

encDec::encDec(): conHan(NULL){
}

encDec::~encDec() {
	// TODO Auto-generated destructor stub
}

/*
endDec::encDec(const encDec& enc){

}
*/

int encDec::Getsizeofpacket(){
	return sizeofpacket;
}

char* encDec::sendFunction(string& line){
	string command("");
	char* ans = NULL;
	unsigned int index=0;
	while ((line.size()>index)&&(line.at(index)!=' ')){
		command=command+line.at(index);
		index++;
	}

	if ((command=="LOGRQ")&&(line.size()>5)){
		ans=CommonPacketWithString(7,line.substr(index+1));
		if (ans!=NULL){
		encDec::shortToBytes(7,ans);
		waitForLog=true;
		}
	}

	else if ((command=="DELRQ")&&(line.size()>5)){
		ans=CommonPacketWithString(8,line.substr(index+1));
		encDec::shortToBytes(8,ans);
	}

	else if ((command=="RRQ")&&(line.size()>3)){
		nameOfFile=line.substr(index+1);
		if (nameOfFile.size()>0){
		ans=CommonPacketWithString(1,nameOfFile);

		encDec::shortToBytes(1,ans);
		}
		else{
			cout << "Wrong read command" << endl;
		}
	}

	else if ((command=="WRQ")&&(line.size()>3)){
		nameOfFile=line.substr(index+1);
		if (nameOfFile.size()>0){
		wannaWrite=true;
		ans=CommonPacketWithString(2,nameOfFile);
		encDec::shortToBytes(2,ans);
		}
		else{
			cout << "wrong write command" << endl;
		}
	}

	else if (command=="DIRQ"){
		ans= new char[2];
		encDec::shortToBytes(6,ans);
		expectDir=true;
		sizeofpacket=2;
	}

	else if (command=="DISC"){
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


char* encDec::CommonPacketWithString(short opCode, string myLine){
	if (myLine.size()<1){
		cout << "wrong command was entered" <<endl;
	}
	else{
		sizeofpacket = 2+myLine.length()+1;
		char* packet = new char[sizeofpacket];
		char opCodeBytes[2];
		shortToBytes(opCode,opCodeBytes);
		packet[0]=opCodeBytes[0];
		packet[1]=opCodeBytes[1];
		char* name = encDec::stringToBytes(myLine);
		for(unsigned int i=0;i<myLine.length();i++){
			packet[i+2]=name[i];
		}
		packet[2+myLine.length()]='0';

		return packet;
	}

	return NULL;
}


char* encDec::stringToBytes(std::string myLine){
	char* bytes = new char[myLine.length()+1];
	for(unsigned int i=0; i<myLine.length();i++)
		bytes[i] = myLine[i];
	bytes[myLine.length()] = '0';
	return bytes;
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

char* encDec::decode(std::string& bytes,ConnectionHandler* conHan){
char* bytearr = new char[2];
	bytearr[0]=bytes.at(0);
	bytearr[1]=bytes.at(1);
	short OP2 = bytesToShort(bytearr);
	switch (OP2){
	case 3:
		if (expectDir){
			handleDIR(bytes);
			expectDir=false;
		}
		else{
			handleFileRead(bytes,conHan);
		}
		break;
	case 4:
		bytearr[0]=bytes[2];
		bytearr[1]=bytes[3];
		ACKblock=bytesToShort(bytearr);
		cout <<"ACK "<< ACKblock <<endl;
		if (waitForDisconnect){
			bytearr[0]=-1;
		}
		else if ((wannaWrite)&(ACKblock==0)){
					handleFileWrite(conHan);
					wannaWrite=false;
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

	if (whichOne==1)
		msg1="add";
	else if (whichOne==0)
		msg1="del";
	string msg2 = bytes.substr(3);

	cout << "BCAST <" << msg1 << "><" << msg2 << ">" << endl;
}

void encDec::handleDIR(std::string& bytes){
	string name;
	char end = '\0';
	char cur;
	unsigned int index=6;
	while (index<bytes.size()){
		cur=bytes.at(index);
		index++;
		if (cur==end){
			cout << name << endl;
		name="";
		}
		else
			name.append(1,cur);
	}
}

void encDec::handleError(std::string& bytes){
	char* bytearr = new char[2];
	bytearr[0]=bytes[2];
	bytearr[1]=bytes[3];
	int errorCode = bytesToShort(bytearr);

	string ErrorDesc = bytes.substr(4,bytes.size());
	cout << "Error" << errorCode << " - " <<ErrorDesc <<endl;
}
void encDec::handleFileWrite(ConnectionHandler* conHan){
	try{
	wannaWrite=false;
	string newname = "Files/"+nameOfFile;
	const char* name = newname.c_str();
	std::ifstream file(name , std::ios::binary);
	file.unsetf(std::ios::skipws);
	std::streampos fileSize;
	file.seekg(0,std::ios::end);
	fileSize=file.tellg();
	file.seekg(0,std::ios::beg);

	vector<char> fileInVector;
	fileInVector.reserve(fileSize+1);
	fileInVector.insert(fileInVector.begin(), std::istream_iterator<char>(file),std::istream_iterator<char>());
	int block=1;
	vector<char> packet;
	vector<char> curData;

	while (!fileInVector.empty()){
		char* OpIn = new char[2];
		shortToBytes(3,OpIn);
		packet.push_back(OpIn[0]);
		packet.insert(packet.end(),OpIn[1]);
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
		shortToBytes(curData.size(),sizeIn);
		packet.insert(packet.end(),sizeIn[0]);
		packet.insert(packet.end(),sizeIn[1]);
		char* blockIn = new char[2];
		shortToBytes(block,blockIn);

		block++;
		packet.insert(packet.end(),blockIn[0]);
		packet.insert(packet.end(),blockIn[1]);
        packet.insert(packet.end(),curData.begin(),curData.end());
		conHan->sendBytes(&packet[0],packet.size());


		packet.clear();
		curData.clear();
		}
	}


	catch(exception e){
		cout << "Writing error occurred. please enter another command"<<endl;
		wannaWrite=false;
	}

}

void encDec::handleFileRead(std::string& bytes,ConnectionHandler* conHan){
	char* bytearr = new char[2];
	bytearr[0]=bytes[2];
	bytearr[1]=bytes[3];
	int block = bytesToShort(bytearr);
	bytearr[0]=bytes[4];
	bytearr[1]=bytes[5];
	int size =bytesToShort(bytearr);
	bytes = bytes.substr(6);

	if (block==lastblockofdata+1){
		lastblockofdata++;
		fileToRead.insert(fileToRead.end(),bytes.begin(),bytes.end());
		char* approve = makeACK(block);
		conHan->sendBytes(approve,4);
		if (size<512){
			string path="Files/"+nameOfFile;
			const char* name = path.c_str();
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

bool encDec::waitForDisc(){
	return waitForDisconnect;
}




