//
//  pool_connection.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_connection_hpp
#define pool_connection_hpp

#include <stdio.h>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../jwumq/jwumq_message.hpp"
#include "../jwumq/jwumq_dealer.hpp"
#include "libjwumqa_cpp.h"
#include "../include/jwevents/jwevents.h"


using namespace std;

using CREATE_PART_PIPE_RESP_DELEGATE = std::function<void(uint32_t)>;
using CREATE_PART_PIPE_RESP_EVENT = JwEvents<CREATE_PART_PIPE_RESP_DELEGATE>;
using CREATE_PART_MEMBER_RESP_DELEGATE = std::function<void(uint32_t)>;
using CREATE_PART_MEMBER_RESP_EVENT = JwEvents<CREATE_PART_MEMBER_RESP_DELEGATE>;
using CREATE_PART_PRODUCER_RESP_DELEGATE = std::function<void(uint32_t)>;
using CREATE_PART_PRODUCER_RESP_EVENT = JwEvents<CREATE_PART_PRODUCER_RESP_DELEGATE>;
using CREATE_PART_CONSUMER_RESP_DELEGATE = std::function<void(uint32_t)>;
using CREATE_PART_CONSUMER_RESP_EVENT = JwEvents<CREATE_PART_CONSUMER_RESP_DELEGATE>;


class PoolConnection
{
public:
	PoolConnection(string url, string queue_name, string node_id);
	~PoolConnection(void);
	
	int Connect();
	void Close();
	int CreatePartPipe(int timeout);
	int CreatePartMember(int timeout);
	int CreatePartProducer(int timeout);
	int CreatePartConsumer(int timeout);
	
	int Send(JMessage * message, int type = 0);
	JMessage * Receive(int timeout);
private:
	int RecvCallback(void * recv_msg);
	int Setup();
	int RequestAlive(int timeout);
	void SendThread();
	void AliveThread();
public:
	CREATE_PART_PIPE_RESP_EVENT create_part_pipe_resp_event;
	CREATE_PART_MEMBER_RESP_EVENT create_part_member_resp_event;
	CREATE_PART_PRODUCER_RESP_EVENT create_part_producer_resp_event;
	CREATE_PART_CONSUMER_RESP_EVENT create_part_consumer_resp_event;
private:
	bool loop;
	string url;
	string queue_name;
	string identity;
	unique_ptr<JwumqDealer> conn;
	
	mutex req_part_pipe_mutex;
	condition_variable req_part_pipe_cv;
	mutex req_part_member_mutex;
	condition_variable req_part_member_cv;
	mutex req_part_producer_mutex;
	condition_variable req_part_producer_cv;
	mutex req_part_consumer_mutex;
	condition_variable req_part_consumer_cv;
	
	mutex send_mutex;
	thread send_thread;
	queue <shared_ptr<JwumqMessage>> send_q;
	
	mutex recv_mutex;
	queue <shared_ptr<JwumqMessage>> recv_q;
	
	thread alive_thread;
	
};

#endif /* pool_connection_hpp */
