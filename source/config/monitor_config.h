//
//  monitor_config.hpp
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/25.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#ifndef monitor_congif_hpp
#define monitor_congif_hpp

#include <stdio.h>
#include "config.h"


using POOL_PROCESS_S = struct pool_process_s
{
	string id;
	string name;
	bool enable;
	uint32_t access_port;
	uint32_t max_queue_counts;
};
using FACTORY_PROCESS_S = struct factory_process_s
{
	string id;
	string name;
	bool enable;
	uint32_t access_port;
	uint32_t max_pool_counts;
};

class MonitorConfig:public Config
{
public:
    MonitorConfig(string conf_file);
    ~MonitorConfig(void);

	bool Parse();
	string Print();
	void UpdatePoolList(string json_str);
private:
	
public:
	list<POOL_PROCESS_S> pool_process_list;
	FACTORY_PROCESS_S factory_config;
private:
	
};

#endif /* monitor_congif_hpp */
