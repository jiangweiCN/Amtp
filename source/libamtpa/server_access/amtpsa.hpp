//
//  amtpsa.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/14.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtpsa_hpp
#define amtpsa_hpp

#include <stdio.h>
#include <memory>
#include "../libamtpa_define.h"
#include "../../jwumq_external/jwumq_define.h"
#include "../../jwumq_external/jwumq_dealer.hpp"
#include <atomic>
#include <functional>

#if defined(_WIN32)
#include "zmq.h"
#else
#include "/home/libdev/zmq/include/zmq.h"
#endif

using namespace std;

//using LIB_JWUMQ_VERSION = char* (*)();
//using LIB_JWUMQ_DEALER_LOAD = void* (*)();
//using LIB_JWUMQ_SETUP = int (*)(void *, JWUMQ_SETUP_CONF_T * , JWUMQ_RECV_CALLBACK );
//using LIB_JWUMQ_SEND = int (*)(void *, void * );
//using LIB_JWUMQ_RELEASE = void (*)(void *);

class Amtpsa
{
public:
	Amtpsa();
	~Amtpsa(void);

	int Connection(char *mq_id, char *cmd_address, char *data_address);
	void Release();
	int SendCmd(uint32_t cmd, char *des_id, void *data, int data_len);
	int ReadCmd(char *src_id, void *data, int data_len);
	int WaitForCmd(int *msg_len, int timeout);
	int SetLimit(int max_size);

private:
	int RecvCmdCallback(void *msg);
	int RecvDataCallback(void *msg);
	void SetupInprocMq(char *mq_id);

public:
private:
	unique_ptr<JwumqDealer> cmd_delear;
	unique_ptr<JwumqDealer> data_delear;
	char mq_id[MAX_IDENTITY_BUF_SIZE];
	// volatile int thread_loop;
	// thread msg_handler_thread;
	volatile int queue_size;
	JwumqMessage *msg_for_read;

	void *inproc_context;
	void *send_socket;
	void *recv_socket;
	char inproc_send_id[MAX_IDENTITY_BUF_SIZE];
	char inproc_recv_id[MAX_IDENTITY_BUF_SIZE];
	char inproc_address[MAX_ADDRESS_BUF_SIZE];
};
#endif /* amtpsa_hpp */
