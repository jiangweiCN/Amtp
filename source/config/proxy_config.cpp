//
//  proxy_config.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "proxy_config.hpp"
#include <algorithm>
#include "../include/cjson/cJSON.h"
#include "../jlog/jLog.h"


ProxyConfig::ProxyConfig(string conf_file, string id)
{
	this->process_id = id;
	this->config_p = nullptr;
	this->config_file = conf_file;
}

ProxyConfig::~ProxyConfig()
{
	config_p = nullptr;
}

bool ProxyConfig::Parse()
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

	cJSON *mq_list = cJSON_GetObjectItem(json, "mq");
	cJSON *iproc_list = cJSON_GetObjectItem(mq_list, "inter process");
	cJSON *stat_monitor = cJSON_GetObjectItem(iproc_list, "stat monitor");
	mq_conf.inter_process_conf.stat_conf.notice_address = string(cJSON_GetObjectItem(stat_monitor,"notice address")->valuestring);
	mq_conf.inter_process_conf.stat_conf.report_address = string(cJSON_GetObjectItem(stat_monitor,"report address")->valuestring);

	cJSON *router = cJSON_GetObjectItem(json, process_id.c_str());
	
	config_p = nullptr;
	config_p = make_unique<PROXY_CONFIG_S>();
	
	config_p->frontend_type = string(cJSON_GetObjectItem(router, "frontend type")->valuestring);
	config_p->backend_type = string(cJSON_GetObjectItem(router, "backend type")->valuestring);
	config_p->frontend_bind = cJSON_GetObjectItem(router, "frontend bind")->valueint;
	config_p->backend_bind = cJSON_GetObjectItem(router, "backend bind")->valueint;
	config_p->frontend_address = string(cJSON_GetObjectItem(router, "frontend address")->valuestring);
	config_p->backend_address = string(cJSON_GetObjectItem(router, "backend address")->valuestring);

	transform(config_p->frontend_type.begin(),config_p->frontend_type.end(),config_p->frontend_type.begin(),::tolower);
	transform(config_p->backend_type.begin(),config_p->backend_type.end(),config_p->backend_type.begin(),::tolower);
	
	cJSON_Delete(json);

	free(data);
	return true;
}

string ProxyConfig::Print()
{
	string p = "";
	if(config_p)
	{
		p = "Config-------frontend address = \""
		+ config_p->frontend_address
		+ "\", backend address = \""
		+ config_p->backend_address + "\""
		+ "\", frontend type = \""
		+ config_p->frontend_type + "\""
		+ "\", backend type = \""
		+ config_p->backend_type + "\""
		+ "\", frontend bind = \""
		+ (config_p->frontend_bind ? "true" : "false") + "\""
		+ "\", backend bind = \""
		+ (config_p->backend_bind ? "true" : "false") + "\"";
		
	}
	return p;
}

