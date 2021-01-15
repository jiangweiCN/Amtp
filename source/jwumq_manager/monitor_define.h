//
//  monitor_define.h
//  Amtp
//
//  Created by jiangwei on 2020/5/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef monitor_define_h
#define monitor_define_h

constexpr auto LIB_JWUMQ_NAME = "libjwumq.so";
using LIB_JWUMQ_VERSION = char* (*)();
using LIB_JWUMQ_DEALER_LOAD = void* (*)();
using LIB_JWUMQ_SETUP = int (*)(void *, JWUMQ_SETUP_CONF_T * , JWUMQ_RECV_CALLBACK );
using LIB_JWUMQ_SEND = int (*)(void *, void * );
using LIB_JWUMQ_RELEASE = void (*)(void *);

enum class MONITOR_RESULT_VALUE_ENUM : int
{
	success = 0x00,
	open_lib_failed,
	not_find_fun_in_lib,
};
using RESULT = MONITOR_RESULT_VALUE_ENUM;

#endif /* monitor_define_h */
