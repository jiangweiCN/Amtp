//
//  factory.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef factory_hpp
#define factory_hpp

#include <stdio.h>
#include <string>
#include <memory>
#include "pool_monitor.hpp"
#include "factory_status.hpp"
#include "access_unit.hpp"

using namespace std;

using FACTORY_CONF_T = struct factory_conf_s
{
	string id;
	int32_t max_pool_counts;
	int32_t access_port;
};

class Factory
{
public:
//	Factory();
	Factory(FACTORY_CONF_T conf);
	~Factory(void);
	
	bool Start();
	bool End();
private:

public:

	
private:
	FACTORY_CONF_T conf;
	unique_ptr<PoolMonitor> pool_monitor;
	unique_ptr<FactoryStatus> factory_status;
	unique_ptr<AccessUnit> access_unit;

};

#endif /* factory_hpp */
