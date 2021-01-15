//
//  pool_def.h
//  Amtp
//
//  Created by jiangwei on 2020/6/12.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef pool_def_h
#define pool_def_h


#include <map>
#include <mutex>
#include <memory>
#include <queue>


using namespace std;

#define JWUMQ_SUCCESS				0

using PART_STATUS_T = struct part_status_s
{
	string node_id;
	uint32_t packets;
	uint32_t bytes;
};

using QUEUE_PART_INFO_MAP = map<string, PART_STATUS_T>;

#endif /* pool_def_h */
