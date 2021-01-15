//
//  access_unit.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/22.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef access_unit_hpp
#define access_unit_hpp

#include <stdio.h>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include "pool_def.h"
#include "../jwumq/jwumq_message.hpp"
#include "../jwumq/jwumq_router_server.hpp"
#include "../include/jwevents/jwevents.h"
#include "pool_status.hpp"


using namespace std;

using CREATE_PART_PIPE_DELEGATE = std::function<void (string, string, int &)>;
using CREATE_PART_PIPE_EVENT = JwEvents<CREATE_PART_PIPE_DELEGATE>;
using CREATE_PART_MEMBER_DELEGATE = std::function<void (string, string, int &)>;
using CREATE_PART_MEMBER_EVENT = JwEvents<CREATE_PART_MEMBER_DELEGATE>;
using CREATE_PART_PRODUCER_DELEGATE = std::function<void (string, string, int &)>;
using CREATE_PART_PRODUCER_EVENT = JwEvents<CREATE_PART_PRODUCER_DELEGATE>;
using CREATE_PART_CONSUMER_DELEGATE = std::function<void (string, string, int &)>;
using CREATE_PART_CONSUMER_EVENT = JwEvents<CREATE_PART_CONSUMER_DELEGATE>;

using RECV_PUBLIC_TEXT_DELEGATE = std::function<void (shared_ptr<JwumqMessage>)>;
using RECV_PUBLIC_TEXT_EVENT = JwEvents<RECV_PUBLIC_TEXT_DELEGATE>;

using RECV_PUBLIC_BYTES_DELEGATE = std::function<void (shared_ptr<JwumqMessage>)>;
using RECV_PUBLIC_BYTES_EVENT = JwEvents<RECV_PUBLIC_BYTES_DELEGATE>;

class AccessUnit
{
public:
	AccessUnit(string id, int port, shared_ptr<PoolStatus> status);
	~AccessUnit(void);
	
	bool Start();
	bool End();
	int Send(shared_ptr<JwumqMessage>);
private:
	int Setup();
	void SendThread();
	void RecvThread();
	int RecvCallback(void * recv_msg);
public:
	CREATE_PART_PIPE_EVENT create_part_pipe_event;
	CREATE_PART_MEMBER_EVENT create_part_member_event;
	CREATE_PART_PRODUCER_EVENT create_part_producer_event;
	CREATE_PART_CONSUMER_EVENT create_part_consumer_event;
	RECV_PUBLIC_TEXT_EVENT recv_public_text_event;
	RECV_PUBLIC_BYTES_EVENT recv_public_bytes_event;
private:
	string pool_id;
	string identity;
	int access_port;
	shared_ptr<PoolStatus> status;
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
