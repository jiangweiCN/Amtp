//
//  pool.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "pool.hpp"


Pool::Pool(POOL_CONF_T conf)
{
	this->conf = conf;
	JLOG(INFO) << "Pool config id: " << this->conf.id.c_str()
		<< ", max_queue_counts: " << this->conf.max_queue_counts
		<< ", access_port: " << this->conf.access_port;
	
	status = make_shared<PoolStatus>();
}

Pool::~Pool()
{
	status.reset();
}

bool Pool::Start()
{
	JLOG(INFO) <<  "Pool start.";
	
	pool_branch = make_unique<PoolBranch>(conf.id, conf.access_port, status);
	if(!pool_branch->Start())
	{
		JLOG(INFO) << "Pool branch start failed! ";
		return false;
	}
	
	queue_center = make_unique<QueueCenter>(conf.id, conf.access_port, status);
	pool_branch->assigned_queue_event += std::bind(&QueueCenter::AssignedQueueListener, queue_center.get(), std::placeholders::_1);
	if(!queue_center->Start())
	{
		JLOG(INFO) << "Queue center start failed! ";
		return false;
	}
	return true;
}

bool Pool::End()
{
	pool_branch->End();
	pool_branch.reset();
	queue_center->End();
	queue_center.reset();
	
	return true;
}



