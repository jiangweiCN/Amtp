//
//  monitor.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "monitor.hpp"
#include <time.h>
#include <signal.h>
#include "../jlog/jLog.h"
#include "../jwumq/jwumq_message.hpp"


Monitor::Monitor()
{
	conf_name = "conf/monitor_config.conf";
	config = make_unique<MonitorConfig>(conf_name);
	ext_m = make_unique<ExternalManage>();
	ext_web_handle = ext_m->web_msg_event += std::bind(&Monitor::WebMsgListener, this, std::placeholders::_1);
}

Monitor::~Monitor()
{
	config = nullptr;
}

bool Monitor::LoadConfig()
{
	JLOG(INFO) <<  "Load config: " << conf_name.c_str();
	bool res = config->Parse();
	if(!res)
	{
		config = nullptr;
		return false;
	}
	JLOG(INFO) << config->Print().c_str();

	return true;
}

bool Monitor::Start()
{
	JLOG(INFO) <<  "Monitor start.";
	bool result = LoadConfig();
	JLOG(INFO) <<  "Load config result: " << result;

	ext_m->Start();
	
	process_check_loop = 1;
	process_monitor_thread = thread(&Monitor::ProcessMonitorThread, this);
	return true;
}

bool Monitor::End()
{
	process_check_loop = 0;
	if(process_monitor_thread.joinable())
	{
		process_monitor_thread.join();
	}
	return true;
}

void Monitor::ProcessMonitorThread()
{
	JLOG(INFO) <<  "Process monitor thread start." ;

	process_list.clear();
	PROCESS_S factory;
	factory.id = config->factory_config.id;
	factory.name = config->factory_config.name;
	factory.para1 = to_string(config->factory_config.max_pool_counts);
	factory.para2 = to_string(config->factory_config.access_port);
	process_list.push_back(factory);
	
	for(auto& p : config->pool_process_list)
	{
		PROCESS_S pool;
		pool.id = p.id;
		pool.name = p.name;
		pool.para1 = to_string(p.max_queue_counts);
		pool.para2 = to_string(p.access_port);
		process_list.push_back(pool);
	}
	
	for(auto& process_s : process_list)
	{
		JLOG(INFO) <<  "Monitor process info, name: " << process_s.name << ", id:" << process_s.id
		 << ", para1:" << process_s.para1 << ", para2:" << process_s.para2;
	}

	int interval = 2;

	time_t l_t = 0, c_t = 0;
	while (process_check_loop > 0)
	{
		c_t = time(NULL);
		if(c_t - l_t >= interval)
		{
			l_t = c_t;
			ProcessCheck();
		}
		usleep(1000*100);
	}

	char cmd[128] = {0};
	sprintf(cmd,"killall -9 jwumq_factory jwumq_pool");
	FILE *f = popen(cmd, "r");//打开管道，执行shell 命令
	pclose(f);
	
	JLOG(INFO) <<  "Process monitor thread exit." ;
}

void Monitor::ProcessCheck()
{
	for(auto& process_s : process_list)
	{
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
			JLOG(INFO) <<  "Start process :" << process_s.name << " " << process_s.id << " " << process_s.para1.c_str() << " " << process_s.para2.c_str();
			char cmd[128] = {0};
			sprintf(cmd,"./%s %s %s %s &", process_s.name.c_str(), process_s.id.c_str(), process_s.para1.c_str(), process_s.para2.c_str());
			FILE *f = popen(cmd, "r");//打开管道，执行shell 命令
			pclose(f);
			usleep(1000*100);
		}
	}
}

void Monitor::WebMsgListener(void * msg)
{
	unique_ptr<JwumqMessage> recv_msg = make_unique<JwumqMessage>((JwumqMessage*)msg);
	switch(recv_msg->body.command())
	{
		case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_request_config):
		{
			ext_m->SendConfig(config.get());
			break;
		}
		case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_update_config):
		{
			unique_ptr<unsigned char[]> buf(new unsigned char[recv_msg->RawDataLen() + 1]());
			memcpy(buf.get(), recv_msg->RawData(), recv_msg->RawDataLen());
			buf[recv_msg->RawDataLen()] = 0;
			config->UpdatePoolList((char*)buf.get());
			JLOG(INFO) <<  "private_update_config, length: "
						<< recv_msg->RawDataLen() << ", content:"
						<< (char*)buf.get();
			LoadConfig();
			
			process_check_loop = 0;
			if(process_monitor_thread.joinable())
			{
				process_monitor_thread.join();
			}
			process_check_loop = 1;
			process_monitor_thread = thread(&Monitor::ProcessMonitorThread, this);
			break;
		}
		default:break;
	}
}
