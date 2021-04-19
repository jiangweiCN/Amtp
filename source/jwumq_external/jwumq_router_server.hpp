//
//  jwumq_router_server.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_router_server_hpp
#define jwumq_router_server_hpp

#include <stdio.h>
#include <thread>
#include <mutex>
#include "jwumq_base.hpp"



class JwumqRouterServer:public JwumqBase
{
public:
	JwumqRouterServer();
	~JwumqRouterServer(void);
	
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	void Release();
	int Send(void * msg);
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
	int Send(int command, char * from, char * to, void * data, int data_len);
private:
	int Setup();
	void RecvThread();
public:
	
private:
	void * socket;
	thread recv_thread;
	
	unique_ptr<ROUTER_SERVER_CONF_T> conf_p;
	char identity[MAX_IDENTITY_BUF_SIZE];
};


#endif /* jwumq_router_server_hpp */
