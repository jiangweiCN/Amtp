//
//  JTextMessageImpl.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef JTextMessageImpl_hpp
#define JTextMessageImpl_hpp

#include <stdio.h>
//#include <vector>
#include "libjwumqa_cpp.h"


class JTextMessageImpl:public JTextMessage
{
public:
	JTextMessageImpl(string text);
	JTextMessageImpl();
	~JTextMessageImpl(void);
	
	void SetText(string text);
	string GetText();
	int GetLength();
	void SetSource(string source);
	void SetDestination(string destination);
	string GetDestination();
	string GetSource();
private:

private:
	string text;
};

#endif /* JTextMessageImpl_hpp */
