//
//  jwumq_dealer.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_dealer_hpp
#define jwumq_dealer_hpp

#include <stdio.h>
#include <thread>
#include "jwumq_base.hpp"

class JwumqDealer:public JwumqBase
{
public:
	JwumqDealer();
	~JwumqDealer(void);
	
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
	void Release();
	int Send(void * msg);
	int Send(int command, char * from, char * to, void * data, int data_len);
private:
	void RecvThread();
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf);
	thread monitor_thread;
public:
	
private:
	unique_ptr<DEALER_CONF_T> conf_p;
	void *socket;
	thread recv_thread;
	char identity[MAX_IDENTITY_BUF_SIZE];
};

#endif /* jwumq_dealer_hpp */
