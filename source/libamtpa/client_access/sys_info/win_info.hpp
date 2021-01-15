//
//  win_info.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/31.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef win_info_hpp
#define win_info_hpp

#include <stdio.h>
#include "system_info.hpp"


class WinInfo:public SysInfo
{
public:
	WinInfo();
	~WinInfo(void);
	
	SYS_INFO_T Get();
private:
	
public:
	
private:
	
};

#endif /* win_info_hpp */
