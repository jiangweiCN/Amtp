//
//  factory_def.h
//  Amtp
//
//  Created by jiangwei on 2020/6/12.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef factory_def_h
#define factory_def_h


#include <string>

using namespace std;

using FACTORY_STATUS_T = struct factory_status_s
{
	uint32_t private_bytes_sum;
	uint32_t private_count_sum;
	uint32_t bytes_sum;
	uint32_t package_sum;
	uint32_t pool_sum;
	uint32_t queue_sum;
};

using POOL_INFO_T = struct pool_info_s
{
	string name;
	uint32_t access_port;
};

using POOL_STATUS_T = struct pool_status_s
{
	string name;
	uint32_t bytes;
	uint32_t packages;
	uint32_t bytes_sum;
	uint32_t package_sum;
	uint32_t queue_sum;
	uint64_t report_time;
	float flow;
};

using QUEUE_INFO_T = struct queue_info_s
{
	string pool_name;
	string queue_name;
	uint32_t access_port;
};


using QUEUE_STATUS_T = struct queue_status_s
{
	string queue_name;
	uint32_t bytes;
	uint32_t packages;
	uint32_t bytes_sum;
	uint32_t package_sum;
	uint64_t report_time;
	float flow;
};

using QUEUE_NAME = string;
using POOL_NAME = string;

constexpr auto QUERY_STATUS_INTERVAL_SEC = 60;


#endif /* factory_def_h */
