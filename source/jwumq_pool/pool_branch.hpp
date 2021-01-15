//
//  pool_branch.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_branch_hpp
#define pool_branch_hpp

#include <stdio.h>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include "../jwumq/jwumq_duplex_sub.hpp"
#include "pool_status.hpp"
#include "../include/jwevents/jwevents.h"


using namespace std;
using ASSIGNED_QUEUE_DELEGATE = std::function<void(string)>;
using ASSIGNED_QUEUE_EVENT = JwEvents<ASSIGNED_QUEUE_DELEGATE>;

class PoolBranch
{
public:
	PoolBranch(string id, uint32_t port, shared_ptr<PoolStatus> status);
	~PoolBranch(void);
	
	bool Start();
	bool End();
private:
	void SendThread();
	int RecvCallback(void * recv_msg);
	void SendStatus(int sn, string des_id);
public:
	ASSIGNED_QUEUE_EVENT assigned_queue_event;
private:
	unique_ptr<JwumqDuplexSub> jwumq_ds;
	string pool_id;
	uint32_t access_port;
	bool loop;
	thread send_thread;
	mutex send_mutex;
	queue <void *> send_q;
	shared_ptr<PoolStatus> status;
};



#endif /* pool_branch_hpp */
