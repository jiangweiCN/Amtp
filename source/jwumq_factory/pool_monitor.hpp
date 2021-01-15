//
//  pool_monitor.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_monitor_hpp
#define pool_monitor_hpp

#include <stdio.h>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include "../jwumq/jwumq_duplex_pub.hpp"
#include "../include/jwevents/jwevents.h"


using namespace std;

using POOL_STATUS_REPORT_DELEGATE = std::function<void(shared_ptr<JwumqMessage>)>;
using POOL_STATUS_REPORT_EVENT = JwEvents<POOL_STATUS_REPORT_DELEGATE>;


class PoolMonitor
{
public:
	PoolMonitor(string id);
	~PoolMonitor(void);
	
	bool Start();
	bool End();
	void AssignedQueue(string, uint64_t, string);
private:
	void StatusQueryThread();
	void SendThread();
	void RecvThread();
	int RecvCallback(void * recv_msg);
public:
	POOL_STATUS_REPORT_EVENT pool_status_report_event;
private:
	unique_ptr<JwumqDuplexPub> jwumq_dp;
	string factory_id;
	bool loop;
	thread status_query_thread;
	thread send_thread;
	thread recv_thread;
	uint32_t sn;
	mutex send_mutex;
	queue <void *> send_q;
	mutex recv_mutex;
	queue <shared_ptr<JwumqMessage>> recv_q;
};


#endif /* pool_monitor_hpp */
