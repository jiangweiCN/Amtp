//
//  amtp_monitor_config.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_monitor_config.hpp"
#include "../include/cjson/cJSON.h"
#include "../jlog/jLog.h"


AmtpMonitorConfig::AmtpMonitorConfig(string conf_file)
{
	this->process_id = "";
	this->config_file = conf_file;
}

AmtpMonitorConfig::~AmtpMonitorConfig()
{
    
}

bool AmtpMonitorConfig::Parse()
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
	log_path = string(cJSON_GetObjectItem(json,"log path")->valuestring);
	
	cJSON *taskArry=cJSON_GetObjectItem(json,"process_list");
	cJSON *tasklist=taskArry->child;
	process_list.clear();
	
	while(tasklist != nullptr)
	{
		PROCESS_S process;
		
		process.id = string(cJSON_GetObjectItem(tasklist,"id")->valuestring);
		process.name = string(cJSON_GetObjectItem(tasklist,"name")->valuestring);
		process_list.push_back(process);

		tasklist=tasklist->next;
	}
	
	cJSON *mq_list = cJSON_GetObjectItem(json, "mq");
	cJSON *iproc_list = cJSON_GetObjectItem(mq_list, "inter process");
	cJSON *stat_monitor = cJSON_GetObjectItem(iproc_list, "stat monitor");
	mq_conf.inter_process_conf.stat_conf.notice_address = string(cJSON_GetObjectItem(stat_monitor,"notice address")->valuestring);
	mq_conf.inter_process_conf.stat_conf.report_address = string(cJSON_GetObjectItem(stat_monitor,"report address")->valuestring);
	
	cJSON *monitor = cJSON_GetObjectItem(json, "monitor");
	monitor_conf.system_stat_interval = cJSON_GetObjectItem(monitor,"system stat interval")->valueint;
	monitor_conf.process_stat_interval = cJSON_GetObjectItem(monitor,"process stat interval")->valueint;
	monitor_conf.process_monitor_interval = cJSON_GetObjectItem(monitor,"process monitor interval")->valueint;
	
	if(monitor_conf.system_stat_interval <= 0)
	{
		monitor_conf.system_stat_interval = 60;
	}
	if(monitor_conf.process_monitor_interval <= 0)
	{
		monitor_conf.process_monitor_interval = 1;
	}
	if(monitor_conf.process_stat_interval <= 0)
	{
		monitor_conf.process_stat_interval = 60;
	}
	
	cJSON_Delete(json);
	
	free(data);
	return true;
}

string AmtpMonitorConfig::Print()
{
	char log [256] = {0};
	sprintf(log, "process stat interval = %d\nsystem stat interval = %d\nprocess monitor interval = %d\nlog path = \"%s\""
			, monitor_conf.process_stat_interval
			, monitor_conf.system_stat_interval
			, monitor_conf.process_monitor_interval
			, log_path.c_str());
	return log;
}
