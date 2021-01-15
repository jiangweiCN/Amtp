//
//  jwumq_router.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_router_hpp
#define jwumq_router_hpp

#include <stdio.h>
#include <thread>
#include <mutex>
#include <list>
#include <map>
#include "jwumq_base.hpp"


using TRACE_LIST = list<string>;
using ROUTER_TABLE = map<string, TRACE_LIST >;

class JwumqRouter:public JwumqBase
{
public:
	JwumqRouter();
	~JwumqRouter(void);
	
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	void Release();
	int Send(void * msg);
	int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
	int Send(int command, char * from, char * to, void * data, int data_len);
private:
	void FrontendRecvThread();
	void BackendRecvThread();
	int Setup();
	void UpdateRouterTable(TRACE_LIST trace);
	TRACE_LIST GetTraceInfo(string des_id);
	void PrintRouterTable();
	void PrintRouterTable(string des_id);
public:
	
private:
	thread frontend_recv_thread;
	thread backend_recv_thread;
	thread frontend_monitor_thread;
	thread backend_monitor_thread;
	void * frontend;
	void * backend;
	
	mutex router_table_mutex;
	ROUTER_TABLE router_table;
	
	unique_ptr<ROUTER_CONF_T> conf_p;
	char identity[MAX_IDENTITY_BUF_SIZE];
};

#endif /* jwumq_router_hpp */
