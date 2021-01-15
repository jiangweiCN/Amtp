//
//  amtp_monitor_config.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_monitor_config_hpp
#define amtp_monitor_config_hpp

#include <stdio.h>
#include "config.h"


using PROCESS_S = struct process_s
{
	string id;
	string name;
};

class AmtpMonitorConfig:public Config
{
public:
    AmtpMonitorConfig(string conf_file);
    ~AmtpMonitorConfig(void);

	bool Parse();
	string Print();
private:
	
public:
	list<PROCESS_S> process_list;
	
private:
    
};
#endif /* amtp_monitor_config_hpp */
