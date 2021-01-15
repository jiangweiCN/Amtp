//
//  factory.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "factory.hpp"
#include "../jlog/jLog.h"


Factory::Factory(FACTORY_CONF_T conf)
{
	this->conf = conf;
	JLOG(INFO) << "Factory config id: " << this->conf.id.c_str()
			<< ", max_pool_counts: " << this->conf.max_pool_counts
			<< ", access_port: " << this->conf.access_port;
}

Factory::~Factory()
{

}

bool Factory::Start()
{
	JLOG(INFO) << "Factory start.";
	
	pool_monitor = make_unique<PoolMonitor>(conf.id);
	factory_status = make_unique<FactoryStatus>();
	access_unit = make_unique<AccessUnit>(conf.id, conf.access_port);
	
	pool_monitor->pool_status_report_event += std::bind(&FactoryStatus::PoolStatusListener, factory_status.get(), std::placeholders::_1);
	access_unit->get_available_pool_fun +=  std::bind(&FactoryStatus::GetAvailablePool, factory_status.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	access_unit->assigned_queue +=  std::bind(&PoolMonitor::AssignedQueue, pool_monitor.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	
	if(!pool_monitor->Start())
	{
		JLOG(INFO) << "Pool monitor start failed! ";
		return false;
	}
	if(!access_unit->Start())
	{
		JLOG(INFO) << "Access unit start failed! ";
		return false;
	}

	return true;
}

bool Factory::End()
{
	access_unit->End();
	pool_monitor->End();
	pool_monitor.reset();

	return true;
}

