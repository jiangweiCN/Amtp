//
//  config.hpp
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/26.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#ifndef config_hpp
#define config_hpp

#include <stdio.h>
#include <string>
#include <list>


using namespace std;

using STAT_MONITOR_CONFIG_S = struct stat_monitor_config_s
{
	string notice_address;
	string report_address;
};

using INTER_PROCESS_CONFIG_S = struct inter_process_config_s
{
	STAT_MONITOR_CONFIG_S stat_conf;
};

using INTER_THREAD_CONFIG_S = struct inter_thread_config_s
{
	
};

using MQ_CONFIG_S = struct mq_config_s
{
	INTER_PROCESS_CONFIG_S inter_process_conf;
	INTER_THREAD_CONFIG_S inter_thread_conf;
};

using MONITOR_CONFIG_S = struct monitor_config_s
{
	int system_stat_interval;
	int process_stat_interval;
	int process_monitor_interval;
};


class Config
{
public:
	Config();
	virtual ~Config(void);
	
	bool Load(char ** data);
	bool Save(string content);
	virtual bool Parse() = 0;
	virtual string Print() = 0;
private:
	
public:
	string version;
	string log_path;
	MQ_CONFIG_S mq_conf;
	MONITOR_CONFIG_S monitor_conf;
protected:
	string config_file;
	string process_id;
};

#endif /* config_hpp */


