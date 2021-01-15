//
//  access_unit.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef access_unit_hpp
#define access_unit_hpp

#include <stdio.h>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include "../jwumq/jwumq_message.hpp"
#include "../jwumq/jwumq_router_server.hpp"
#include "../include/jwevents/jwevents.h"


using namespace std;

using GET_AVAILABLE_POOL_DELEGATE = std::function<void (string, string &, int &)>;
using GET_AVAILABLE_POOL_FUN = JwEvents<GET_AVAILABLE_POOL_DELEGATE>;

using ASSIGNED_QUEUE_TO_POOL_DELEGATE = std::function<void (string, uint64_t, string)>;
using ASSIGNED_QUEUE_TO_POOL_FUN = JwEvents<ASSIGNED_QUEUE_TO_POOL_DELEGATE>;

class AccessUnit
{
public:
	AccessUnit(string id, int access_port);
	~AccessUnit(void);
	
	bool Start();
	bool End();
private:
	int Setup();
	void SendThread();
	void RecvThread();
	int RecvCallback(void * recv_msg);
public:
	GET_AVAILABLE_POOL_FUN get_available_pool_fun;
	ASSIGNED_QUEUE_TO_POOL_FUN assigned_queue;
private:
	string factory_id;
	string identity;
	int access_port;
	unique_ptr<JwumqRouterServer> conn;
	bool loop;
	thread send_thread;
	thread recv_thread;
	mutex send_mutex;
	queue <shared_ptr<JwumqMessage>> send_q;
	mutex recv_mutex;
	queue <shared_ptr<JwumqMessage>> recv_q;
};


#endif /* access_unit_hpp */
