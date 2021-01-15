//
//  factory_connection.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef factory_connection_hpp
#define factory_connection_hpp

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
using SESSION_RESP_DELEGATE = std::function<void(uint64_t)>;
using SESSION_RESP_EVENT = JwEvents<SESSION_RESP_DELEGATE>;

using CREATE_QUEUE_RESP_DELEGATE = std::function<void(bool, int)>;
using CREATE_QUEUE_RESP_EVENT = JwEvents<CREATE_QUEUE_RESP_DELEGATE>;


class FactoryConnection
{
public:
	FactoryConnection(string url);
	~FactoryConnection(void);
	
	int Connect();
	void Close();
	int RequestSession(int timeout);
	int CreateQueue(uint64_t session, string queue_name, int timeout);
	string Url();
private:
	int RecvCallback(void * recv_msg);
	int Setup();
	int RequestAlive(int timeout);
	void SendThread();
	void RecvThread();
	void AliveThread();
public:
	SESSION_RESP_EVENT session_resp_event;
	CREATE_QUEUE_RESP_EVENT create_queue_resp_event;
private:
	bool loop;
	string url;
	string identity;
	unique_ptr<JwumqDealer> conn;
	
	mutex req_session_mutex;
	condition_variable req_session_cv;
	mutex req_create_queue_mutex;
	condition_variable req_create_queue_cv;
	
	mutex send_mutex;
	thread send_thread;
	queue <shared_ptr<JwumqMessage>> send_q;
	
	mutex recv_mutex;
	thread recv_thread;
	queue <shared_ptr<JwumqMessage>> recv_q;
	
	thread alive_thread;
};
#endif /* factory_connection_hpp */
