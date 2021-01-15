//
//  monitor_config.cpp
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/25.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#include "monitor_config.h"
#include "../include/cjson/cJSON.h"
#include "../jlog/jLog.h"


MonitorConfig::MonitorConfig(string conf_file)
{
	this->process_id = "";
	this->config_file = conf_file;
}

MonitorConfig::~MonitorConfig()
{
    
}

bool MonitorConfig::Parse()
{
	char *data = nullptr;
	
	if(!Load(&data))
	{
		return false;
	}
	
	cJSON *json = cJSON_Parse(data);
	if(json == nullptr)
	{
		free(data);
		return false;
	}
	version = string(cJSON_GetObjectItem(json,"version")->valuestring);
	log_path = string(cJSON_GetObjectItem(json,"log_path")->valuestring);
	
	cJSON *factory = cJSON_GetObjectItem(json, "jwumq_factory");
	factory_config.id = string(cJSON_GetObjectItem(factory,"id")->valuestring);
	factory_config.name = "jwumq_factory";
	factory_config.enable = cJSON_GetObjectItem(factory,"enable")->valueint;
	factory_config.max_pool_counts = cJSON_GetObjectItem(factory,"max_pool_counts")->valueint;
	factory_config.access_port = cJSON_GetObjectItem(factory,"access_port")->valueint;
	
	cJSON *taskArry=cJSON_GetObjectItem(json,"pool_list");
	cJSON *tasklist=taskArry->child;
	pool_process_list.clear();
	
	while(tasklist != nullptr)
	{
		POOL_PROCESS_S pool;
		
		pool.id = string(cJSON_GetObjectItem(tasklist,"id")->valuestring);
		pool.name = "jwumq_pool";
		pool.enable = cJSON_GetObjectItem(tasklist, "enable")->valueint;
		pool.access_port = cJSON_GetObjectItem(tasklist, "access_port")->valueint;
		pool.max_queue_counts = cJSON_GetObjectItem(tasklist, "max_queue_counts")->valueint;
		pool_process_list.push_back(pool);

		tasklist=tasklist->next;
	}
	
	cJSON_Delete(json);
	
	free(data);
	return true;
}

void MonitorConfig::UpdatePoolList(string json_str)
{
	cJSON *json_update = cJSON_Parse(json_str.c_str());
	if(json_update == nullptr)
	{
		return;
	}
	cJSON *new_taskArry=cJSON_GetObjectItem(json_update,"pool_list");

	cJSON * root =  cJSON_CreateObject();
	cJSON * jwumq_factory_item =  cJSON_CreateObject();

	cJSON_AddItemToObject(root, "version", cJSON_CreateString(version.c_str()));
	cJSON_AddItemToObject(root, "log_path", cJSON_CreateString(log_path.c_str()));
	cJSON_AddItemToObject(root, "jwumq_factory", jwumq_factory_item);

	cJSON_AddItemToObject(jwumq_factory_item, "id", cJSON_CreateString(factory_config.id.c_str()));
	cJSON_AddItemToObject(jwumq_factory_item, "enable", cJSON_CreateNumber(factory_config.enable));
	cJSON_AddItemToObject(jwumq_factory_item, "max_pool_counts", cJSON_CreateNumber(factory_config.max_pool_counts));
	cJSON_AddItemToObject(jwumq_factory_item, "access_port", cJSON_CreateNumber(factory_config.access_port));

	cJSON_AddItemToObject(root, "pool_list", new_taskArry);
	char *cjson_str = NULL;
	cjson_str = cJSON_Print(root);

	Save(cjson_str);
	free(cjson_str);
	cJSON_Delete(root);
//	cJSON_Delete(json_update);
}
string MonitorConfig::Print()
{
	string print;
	std::ostringstream base_os;
	base_os << "version: " << version
			<< ", logpath: " << log_path
			<< "\n";
	print.append(base_os.str());
	
	std::ostringstream factory_os;
	factory_os << "id: " << factory_config.id
				<< ", name: " << factory_config.name
				<< ", access port: " << factory_config.access_port
				<< "\n";
	print.append(factory_os.str());
	
	for(auto pool : pool_process_list)
	{
		std::ostringstream pool_os;
		pool_os << "id: "<< pool.id
		<< ", name: " << pool.name
		<< ", enable: " << pool.enable
		<< ", access port: " << pool.access_port
		<< ", max queue counts: " << pool.max_queue_counts
		<< "\n";
		print.append(pool_os.str());
	}
	
	return print;
}
