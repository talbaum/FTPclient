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
	cout << "sendfuction entered" << endl;
	char* ans = NULL;
	unsigned int index=0;
	while ((line.size()>index)&&(line.at(index)!=' ')){
		command=command+line.at(index);
		index++;
	}

	if ((command=="LOGRQ")&&(line.size()>5)){
		cout << "sendfuction entered LOGRQ " << endl;
		ans=CommonPacketWithString(7,line.substr(index+1));

		if (ans!=NULL){
		encDec::shortToBytes(7,ans);
		waitForLog=true;
		}
	}

	else if ((command=="DELRQ")&&(line.size()>5)){
		cout << "sendfuction entered DELRQ " << endl;
		ans=CommonPacketWithString(8,line.substr(index+1));
		encDec::shortToBytes(8,ans);
	}

	else if ((command=="RRQ")&&(line.size()>3)){
		cout << "sendfuction entered RRQ " << endl;
		nameOfFile=line.substr(index+1);
		cout << "after substr " << nameOfFile<<endl;
		if (nameOfFile.size()>0){
		ans=CommonPacketWithString(1,nameOfFile);

		for(int i=0;i<20;i++){
			cout <<*(ans+i)<<endl;
		}


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
		ans=CommonPacketWithString(2,nameOfFile);
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


char* encDec::CommonPacketWithString(short opCode, string myLine){
	//char* ans= NULL;

	if (myLine.size()<1){
		cout << "wrong command was entered" <<endl;
	}
	else{
		sizeofpacket = 2+myLine.length()+1;
		char* packet = new char[sizeofpacket];
		//unsigned int index=0;
		char opCodeBytes[2];
		shortToBytes(opCode,opCodeBytes);
		packet[0]=opCodeBytes[0];
		packet[1]=opCodeBytes[1];
		char* name = encDec::stringToBytes(myLine);
		for(unsigned int i=0;i<myLine.length();i++){
			packet[i+2]=name[i];
		}
		packet[2+myLine.length()]='0';
		cout << "packer created is this many bytes: "<< 2+myLine.length()+1 << endl;

		return packet;
	}

	return NULL;
}


char* encDec::stringToBytes(std::string myLine){
	char* bytes = new char[myLine.length()+1];
	for(unsigned int i=0; i<myLine.length();i++)
		bytes[i] = myLine[i];
	bytes[myLine.length()] = '0';
/*	bytes.push_back('0');*/
/*	char *c = &bytes[0];*/
	return bytes;
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
			cout << "expectDir " << endl;
			handleDIR(bytes);
			expectDir=false;
		}
		else{
			handleFileRead(bytes,conHan);
			cout <<"not expectDir " << endl;
		}break;
	case 4:
		bytearr[0]=bytes[2];
		bytearr[1]=bytes[3];
		ACKblock=bytesToShort(bytearr);
		cout << "ACK " << ACKblock << endl;

		if (waitForDisconnect){
			//bytes.clear();
/*			this->disconnect=true;*/
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

	if (whichOne==1)
		msg1="add";
	else if (whichOne==0)
		msg1="del";
	string msg2 = bytes.substr(3);

	cout << "BCAST <" << msg1 << "><" << msg2 << ">" << endl;
}

void encDec::handleDIR(std::string& bytes){
	//bytes.substr(6);
	cout << "dir length without OP: " << bytes.size()-6 << endl;
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
	cout << "inside handleFileWrite!" <<endl;
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
	cout << "before reserve! fileSize:" << fileSize << endl;
	fileInVector.reserve(fileSize+1);
	cout << "after reserve! fileSize:" << fileSize << endl;
	fileInVector.insert(fileInVector.begin(), std::istream_iterator<char>(file),std::istream_iterator<char>());
	//fileInVector.push_back('/0');
	int block=1;
	vector<char> packet;
	vector<char> curData;
	//int size;

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

		cout << "sent data packet number: "<< block-1<<endl;
		while (ACKblock!=block-1){
			//wait!!
		}
		packet.clear();
		curData.clear();
	}
	}
	catch(exception e){
		cout << "Writing error occurred. please enter another command"<<endl;
		wannaWrite=false;
	}
	wannaWrite=false;
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

bool encDec::waitForDisc(){
	return waitForDisconnect;
}




