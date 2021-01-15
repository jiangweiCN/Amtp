//
//  libjwumqa_session.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/18.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_session_hpp
#define libjwumqa_session_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "factory_connection.hpp"


class JwumqSessionImpl:public JwumqSession
{
public:
	JwumqSessionImpl();
	~JwumqSessionImpl(void);
	
	JwumqDestination*  CreateDestination(string queue_name);
	void SessionRespListener(uint64_t session);
	JTextMessage* CreateTextMessage(string text);
	JTextMessage* CreateTextMessage();
	JBytesMessage* CreateBytesMessage(unsigned char * data, int length);
	JBytesMessage* CreateBytesMessage();
	void DestroyDestination(JwumqDestination*);
	void DestroyMessage(JMessage * message);
public:
	shared_ptr<FactoryConnection> factory_conn;
private:
	string queue_name;
	uint64_t session;
	
};

#endif /* libjwumqa_session_hpp */
