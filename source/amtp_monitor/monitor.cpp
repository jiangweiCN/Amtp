//
//  monitor.cpp
//  Monitor
//
//  Created by jiangwei on 2018/12/25.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#include "monitor.h"
#include <time.h>
#include <signal.h>
#include "../jlog/jLog.h"


Monitor::Monitor(string conf_file)
{
	loop = 0;
	pthread_mutex_init(&mutex, NULL);
	config = make_unique<AmtpMonitorConfig>(conf_file);
	file_name = conf_file;
}

Monitor::~Monitor()
{
	config = nullptr;
}

bool Monitor::LoadConfig()
{
	pthread_mutex_lock(&mutex);
	
	bool res = config->Parse();
	pthread_mutex_unlock(&mutex);
	if(!res)
	{
		config = nullptr;
		return false;
	}
	
	log_path = config->log_path;
	
	return true;
}

bool Monitor::Start()
{
	loop = 1;
	process_monitor_thread = thread(&Monitor::ProcessMonitorThread, this);
	return true;
}

bool Monitor::End()
{
	loop = 0;
	if(process_monitor_thread.joinable())
	{
		process_monitor_thread.join();
	}
	return true;
}

void Monitor::ProcessMonitorThread()
{
	JLOG(INFO) <<  "Process monitor thread start." ;

	for(auto& process_s : config->process_list)
	{
		JLOG(INFO) <<  "Monitor process info, id:" << process_s.id << ", process name:" << process_s.name;
	}
	
	int interval = config->monitor_conf.process_monitor_interval;
	
	time_t l_t = 0, c_t = 0;
	while (loop > 0)
	{
		c_t = time(NULL);
		if(c_t - l_t >= interval)
		{
			l_t = c_t;
			pthread_mutex_lock(&mutex);
			ProcessCheck();
			pthread_mutex_unlock(&mutex);
		}
		usleep(1000*100);
	}
	
	JLOG(INFO) <<  "Process monitor thread exit." ;
}

void Monitor::ProcessCheck()
{
	for(auto& process_s : config->process_list)
	{
//		JLOG(INFO) <<  "Monitor process info, id:" << process_s.id << ", process name:" << process_s.name;

		char cmd[512] = {0};
		sprintf(cmd, "ps -ef | grep \'%s %s\' | grep -v grep | awk \'{print $2}\'", process_s.name.c_str(), process_s.id.c_str());
		
		FILE *fp = popen(cmd, "r");//打开管道，执行shell 命令
		char buffer[128] = {0};
		bool pro_exist = false;
		while (NULL != fgets(buffer, 128, fp)) //逐行读取执行结果并打印
		{
			printf("PID:  %s", buffer);
			pid_t pID = atoi(buffer);
			int ret = kill(pID,0);
			if(ret == 0)
			{
				pro_exist = true;
			}
		}
		pclose(fp);
		if(!pro_exist)
		{
			JLOG(INFO) <<  "Start process :" << process_s.name << " " << process_s.id << " " << file_name.c_str() << " " << log_path.c_str();
			char cmd[128] = {0};
			sprintf(cmd,"./%s %s %s %s &", process_s.name.c_str(), process_s.id.c_str(), file_name.c_str(), log_path.c_str());
			FILE *f = popen(cmd, "r");//打开管道，执行shell 命令
			pclose(f);
			usleep(100000);
		}
	}
}

