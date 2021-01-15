//
//  JBytesMessageImpl.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef JBytesMessageImpl_hpp
#define JBytesMessageImpl_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"


class JBytesMessageImpl:public JBytesMessage
{
public:
	JBytesMessageImpl(unsigned char * data, int length);
	JBytesMessageImpl();
	~JBytesMessageImpl(void);
	
	void SetBytes(unsigned char * data, int length);
	int GetLength();
	int GetBytes(unsigned char * data, int length);
	uint8_t * GetBytes();
	void SetSource(string source);
	void SetDestination(string destination);
	string GetDestination();
	string GetSource();
private:

private:
	int length;
	unique_ptr<unsigned char[]> data;
};

#endif /* JBytesMessageImpl_hpp */
