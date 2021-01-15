//
//  system_info.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/31.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef system_info_hpp
#define system_info_hpp

#include <stdio.h>
#include <string>

using namespace std;


using SYS_INFO_T = struct sys_info_s
{
	string system;
	string cpu_id;
	string hard_disk;
};

class SysInfo
{
public:
	SysInfo();
	virtual ~SysInfo(void);
	
	virtual SYS_INFO_T Get() = 0;
};


#endif /* system_info_hpp */
