//
//  external_manage.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/14.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef external_manage_hpp
#define external_manage_hpp

#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include "../jwumq/jwumq_define.h"
#include "monitor_define.h"
#include "../include/jwevents/jwevents.h"


using namespace std;

using WEB_MSG_DELEGATE = std::function<void(void *)>;
using WEB_MSG_EVENT = JwEvents<WEB_MSG_DELEGATE>;

class ExternalManage
{
public:
	ExternalManage();
	~ExternalManage(void);
	
	int Setup();
	bool Start();
	bool End();
	void SendConfig(void * config);
private:
	int RecvCallback(void * msg);
	void SendHostInfo();
	void SendUpdateConfigAck();
	void RecvThread();
	void * ReadMsg();
public:
	WEB_MSG_EVENT web_msg_event;
private:
	void *lib_handle;
	LIB_JWUMQ_VERSION lib_version = NULL;
	LIB_JWUMQ_DEALER_LOAD lib_dealer_load = NULL;
	LIB_JWUMQ_SETUP lib_jwumq_setup = NULL;
	LIB_JWUMQ_SEND lib_jwumq_send = NULL;
	LIB_JWUMQ_RELEASE lib_jwumq_release = NULL;
	void * jmq_handle;
	string self_id;
	string web_id;
	
	int loop;
	thread recv_thread;
	mutex recv_mutex;
	queue<void *> recv_queue;
	
};

#endif /* external_manage_hpp */
