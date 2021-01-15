//
//  jwumq_duplex_pub.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_duplex_pub_hpp
#define jwumq_duplex_pub_hpp

#include <stdio.h>
#include <string>
#include <thread>
#include "jwumq_base.hpp"


using namespace std;

class JwumqDuplexPub:public JwumqBase
{
public:
	JwumqDuplexPub();
	~JwumqDuplexPub(void);
	
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
	void Release();
	int Send(void * msg);
	int Send(int command, char * from, char * to, void * data, int data_len);
private:
	void RecvThread();
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf);
public:
	
private:
	unique_ptr<DUPLEX_PUB_SUB_CONF_T> conf_p;
	void *pub_socket;
	void *dealer_socket;
	thread recv_thread;
	char identity[MAX_IDENTITY_BUF_SIZE];
};


#endif /* jwumq_duplex_pub_hpp */
