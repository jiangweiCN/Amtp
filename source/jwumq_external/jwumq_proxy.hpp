//
//  jwumq_proxy.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_proxy_hpp
#define jwumq_proxy_hpp

#include <stdio.h>
#include <thread>
#include <list>
#include "jwumq_base.hpp"


class JwumqProxy:public JwumqBase
{
public:
	JwumqProxy();
	~JwumqProxy(void);
	
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	void Release();
	int Send(void * msg);
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
	int Send(int command, char * from, char * to, void * data, int data_len);
private:
	void ProxyThread();
	int Setup();
public:
	
private:
	thread proxy_thread;
	thread frontend_monitor_thread;
	thread backend_monitor_thread;
	void * frontend;
	void * backend;
	
	unique_ptr<PROXY_CONF_T> conf_p;
	char identity[MAX_IDENTITY_BUF_SIZE];
};


#endif /* jwumq_proxy_hpp */
