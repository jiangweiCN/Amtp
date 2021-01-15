//
//  factory_status.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef factory_status_hpp
#define factory_status_hpp

#include <stdio.h>
#include <memory>
#include <map>
#include <mutex>
#include "../jwumq/jwumq_message.hpp"
#include "factory_def.h"


using QUEUE_INFO_MAP = map<QUEUE_NAME, QUEUE_INFO_T>;
using QUEUE_STATUS_MAP = map<QUEUE_NAME, QUEUE_STATUS_T>;
using POOL_INFO_MAP = map<POOL_NAME, POOL_INFO_T>;
using POOL_STATUS_MAP = map<POOL_NAME, POOL_STATUS_T>;

class FactoryStatus
{
public:
	FactoryStatus();
	~FactoryStatus(void);
	
	void PoolStatusListener(shared_ptr<JwumqMessage>);
	void GetAvailablePool(string, string &, int &);
private:
	void UpdateQueueInfo(string, string, uint32_t);
	void UpdateQueueStatus(string, uint32_t, uint32_t, uint64_t);
	void UpdatePoolInfo(string, uint32_t);
	void UpdatePoolStatus(string, uint32_t, uint32_t, uint32_t, uint64_t);
	
	shared_ptr<QUEUE_INFO_T> FindQueue(string queue_name);
	string GetMinQueueSumPool();
	uint32_t GetPoolAccessPort(string pool_name);
public:
private:
	FACTORY_STATUS_T 	fs;
	QUEUE_INFO_MAP 		queue_info_map;
	QUEUE_STATUS_MAP	queue_status_map;
	POOL_INFO_MAP		pool_info_map;
	POOL_STATUS_MAP 	pool_status_map;
	mutex pool_info_mutex;
	mutex pool_status_mutex;
	mutex queue_info_mutex;
	mutex queue_status_mutex;
};

#endif /* factory_status_hpp */
