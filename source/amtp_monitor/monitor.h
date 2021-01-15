//
//  monitor.hpp
//  Monitor
//
//  Created by jiangwei on 2018/12/25.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#ifndef monitor_h
#define monitor_h

#include <stdio.h>
#include <string>
#include <thread>
#include "../config/amtp_monitor_config.hpp"



using namespace std;

class Monitor
{
public:
	Monitor(string conf_file);
	~Monitor(void);
	
	bool LoadConfig();
	bool Start();
	bool End();
private:
	void ProcessMonitorThread();
	void ProcessCheck();
public:
	unique_ptr<AmtpMonitorConfig> config;
	
private:
	string file_name;
	thread process_monitor_thread;
	pthread_mutex_t mutex;
	int loop;
	string log_path;
};
#endif /* monitor_hpp */
