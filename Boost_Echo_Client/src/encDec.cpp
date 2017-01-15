/*
 * encDec.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: amit
 */

#include "../include/encDec.h"
#include <stdio.h>
#include <string.h>
using namespace std;
char bytes[];
int len=0;
short OP;
encDec::encDec() {
	//this.bytes[] = new char[len];
	OP=0;
}

encDec::~encDec() {
	// TODO Auto-generated destructor stub
}

encDec::encode(char recivedBytes[]){

	this->bytes[] = new char[recivedBytes.size()];
	strcpy (recivedBytes,bytes);
	this->OP = encDec::getOp();
}

encDec::getOp(){
	char OpBytes[2];
	for (int i=0;i<2;i++){
		OpBytes[i]=bytes[i];
	}

return this->bytes[0];
}

encDec::bytesToString(char* recivedBytes,int size){
	char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',   'B','C','D','E','F'};

	  std::string str;
	  for (int i = 0; i < size; ++i) {
	    const char ch = recivedBytes[i];
	    str.append(&hex[(ch  & 0xF0) >> 4], 1);
	    str.append(&hex[ch & 0xF], 1);
	  }
	  return str;
	}

