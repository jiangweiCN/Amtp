//
//  pool_status.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_status_hpp
#define pool_status_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <mutex>
#include "../jwumq_protocol/jwumq_msg.pb.h"

using namespace std;

using POOL_STATUS_T = struct pool_status_s
{
	uint32_t private_bytes;
	uint32_t private_count;
	uint32_t queue_count;
};

using QUEUE_STATUS_T = struct queue_status_s
{
	string name;
	uint32_t total_bytes;
	uint32_t package_count;
	time_t last_time;
};
using QUEUE_STATUS_MAP = map<string, QUEUE_STATUS_T>;

class PoolStatus
{
public:
	PoolStatus();
	~PoolStatus(void);
	
	void Reset(jwumq_pb_msg::PoolStatusReport & pb);
	void UpdatePoolPrivate(uint32_t bytes, uint32_t count);
private:
//	void GetStatus(jwumq_pb_msg::PoolStatusReport &);
public:
	
private:
	POOL_STATUS_T pool_status;
	QUEUE_STATUS_MAP queue_status;
	mutex pool_status_mutex;
	mutex queue_status_mutex;
};
#endif /* pool_status_hpp */




