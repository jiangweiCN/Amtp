//
//  jwumq_base.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_base_hpp
#define jwumq_base_hpp

#include <stdio.h>
#include <string>
#include <memory>
#include <assert.h>
#include "jwumq_define.h"
#if defined(_WIN32)
#include "zmq.h"
#else
#include "/home/libdev/zmq/include/zmq.h"
#endif
#include "jwumq_message.hpp"
#include "../jlog/jLog.h"
#include <map>

using namespace std;

class JwumqBase
{
public:
	JwumqBase();
	virtual ~JwumqBase(void);
	
	virtual int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback) = 0;
	virtual int Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback) = 0;

	virtual void Release() = 0;
	virtual int Send(void * msg) = 0;
	virtual int Send(int command, char * from, char * to, void * data, int data_len) = 0;

protected:
	int ReadMonitorMsg(void *monitor, int *value, string & addr);
	void SocketMonitorThread(char * name, void *ctx, void * s, char * addr, bool log);
	void * ZmqSocket(void * ctx, ZMQ_TYPE_ENUM type);
public:

protected:
	void *context;
	JWUMQ_RECV_CALLBACK recv_callback;
	JWUMQ_RECV_CALLBACK_C recv_callback_c;
	int loop;
private:
	map<ZMQ_TYPE_ENUM, int> type_kv;
	
};
#endif /* jwumq_base_hpp */
