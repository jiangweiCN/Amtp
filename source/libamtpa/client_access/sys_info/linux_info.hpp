//
//  linux_info.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/31.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef linux_info_hpp
#define linux_info_hpp

#include <stdio.h>
#include "system_info.hpp"


class LinuxInfo:public SysInfo
{
public:
	LinuxInfo();
	~LinuxInfo(void);
	
	SYS_INFO_T Get();
private:
	void Cpuid(unsigned int veax1);
	void Lm(int var,uint32_t *vx);
	string GetCpuid ();
	string GetOSInfo();
	string GetHardDiskInfo();
public:
	
private:
	
};

#endif /* linux_info_hpp */
