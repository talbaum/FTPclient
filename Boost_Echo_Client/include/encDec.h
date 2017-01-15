/*
 * encDec.h
 *
 *  Created on: Jan 15, 2017
 *      Author: amit
 */

#ifndef SRC_ENCDEC_H_
#define SRC_ENCDEC_H_

class encDec {
public:
	encDec();
	virtual ~encDec();
	char* encode(char* recivedBytes);
	short getOp();
	char bytes[];
	short OP;
	std::string bytesToString(char* bytes);
};

#endif /* SRC_ENCDEC_H_ */
