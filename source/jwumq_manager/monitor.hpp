//
//  monitor.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef monitor_hpp
#define monitor_hpp

#include <stdio.h>
#include <memory>
#include "../config/monitor_config.h"
#include "external_manage.hpp"
#include "internal_manage.hpp"

using PROCESS_S = struct process_s
{
	string id;
	string name;
	string para1;
	string para2;
};

using namespace std;

class Monitor
{
public:
	Monitor();
	~Monitor(void);
	
	bool Start();
	bool End();
private:
	void ProcessMonitorThread();
	void ProcessCheck();
	bool LoadConfig();
	void WebMsgListener(void * msg);
public:
	unique_ptr<MonitorConfig> config;
	
private:
	unique_ptr<ExternalManage> ext_m;
	unique_ptr<InternalManage> int_m;
	int ext_web_handle;
	string conf_name;
	thread process_monitor_thread;
	int process_check_loop;
	list<PROCESS_S> process_list;
};

#endif /* monitor_hpp */
