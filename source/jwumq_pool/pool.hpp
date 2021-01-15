//
//  pool.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_hpp
#define pool_hpp

#include <stdio.h>
#include <string>
#include "pool_branch.hpp"
#include "pool_status.hpp"
#include "queue_center.hpp"


using namespace std;

using POOL_CONF_T = struct pool_conf_s
{
	string id;
	int32_t max_queue_counts;
	int32_t access_port;
};

class Pool
{
public:
	Pool(POOL_CONF_T conf);
	~Pool(void);
	
	bool Start();
	bool End();
private:

public:
	unique_ptr<PoolBranch> pool_branch;
	shared_ptr<PoolStatus> status;
	unique_ptr<QueueCenter> queue_center;
private:
	POOL_CONF_T conf;

};
#endif /* pool_hpp */
