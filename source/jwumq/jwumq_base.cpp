//
//  jwumq_base.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_base.hpp"
#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#include <errno.h>
#include <string.h>
#include <locale.h>
#else

#endif


JwumqBase::JwumqBase()
{
	this->context = nullptr;
	this->recv_callback = nullptr;
	
	type_kv = {{ZMQ_TYPE_ENUM::pair, ZMQ_PAIR}, {ZMQ_TYPE_ENUM::pub, ZMQ_PUB}
		, {ZMQ_TYPE_ENUM::sub, ZMQ_SUB}, {ZMQ_TYPE_ENUM::req, ZMQ_REQ}
		, {ZMQ_TYPE_ENUM::rep, ZMQ_REP}, {ZMQ_TYPE_ENUM::dealer, ZMQ_DEALER}
		, {ZMQ_TYPE_ENUM::router, ZMQ_ROUTER}, {ZMQ_TYPE_ENUM::pull, ZMQ_PULL}
		, {ZMQ_TYPE_ENUM::push, ZMQ_PUSH}, {ZMQ_TYPE_ENUM::xpub, ZMQ_XPUB}
		, {ZMQ_TYPE_ENUM::xsub, ZMQ_XSUB}, {ZMQ_TYPE_ENUM::stream, ZMQ_STREAM}
	};
}

JwumqBase::~JwumqBase()
{
	
}

int JwumqBase::ReadMonitorMsg(void *monitor, int *value, string & addr)
{
    // First frame in message contains event number and value
	zmq_msg_t msg;
	zmq_msg_init (&msg);
	if (zmq_msg_recv (&msg, monitor, 0) == -1)
		return -1; // Interrupted, presumably
	assert (zmq_msg_more (&msg));

	uint8_t *data = (uint8_t *) zmq_msg_data (&msg);
	uint16_t event = *(uint16_t *) (data);
	if (value)
		*value = *(uint32_t *) (data + 2);
	zmq_msg_close (&msg);
	// Second frame in message contains event address
	zmq_msg_init (&msg);
	if (zmq_msg_recv (&msg, monitor, 0) == -1)
		return -1; // Interrupted, presumably
	assert (!zmq_msg_more (&msg));

	data = (uint8_t *) zmq_msg_data (&msg);
	size_t size = zmq_msg_size (&msg);
	unique_ptr<char[]> buf(new char[size + 1]());
	memcpy (buf.get(), data, size);
	buf[size] = 0;
	addr = buf.get();
	zmq_msg_close (&msg);
	return event;
}

void JwumqBase::SocketMonitorThread(char * name, void *ctx, void * s, char * addr, bool log)
{
	// Create socket for collecting monitor events
	void * monitor = zmq_socket (ctx, ZMQ_PAIR);
	assert (monitor);

	// Connect it to the inproc endpoints so it'll get events
	int rc = zmq_connect (monitor, addr);
	if(rc != 0)
	{
#if defined(_WIN32)
		////size_t errmsglen = strerrorlen_s(errno) + 1;
		////char errmsg[errmsglen];
		////strerror_s(errmsg, errmsglen, errno);
		char errmsg[128];
		strerror_s(errmsg, 128, errno);
		printf("Now in Chinese: %s\n", errmsg);
		JLOG(INFO) << " socket monitor connect error, " << errmsg;
#else
		JLOG(INFO) << " socket monitor connect error, " << strerror(errno);
#endif

		
		return ;
	}
	assert (rc == 0);

	// Now collect and check events from socket
	int value = 0;
	string name_string = name;
	
	while(loop)
	{
		string address;
		int event = ReadMonitorMsg (monitor, &value, address);
		string event_name;
		switch(event)
		{
			case ZMQ_EVENT_CONNECTED:
				event_name = "EVENT_CONNECTED";
				break;
			case ZMQ_EVENT_CONNECT_DELAYED:
				event_name = "EVENT_CONNECTED";
				break;
			case ZMQ_EVENT_CONNECT_RETRIED:
				event_name = "EVENT_CONNECT_RETRIED";
				break;
			case ZMQ_EVENT_LISTENING:
				event_name = "EVENT_LISTENING";
				break;
			case ZMQ_EVENT_BIND_FAILED:
				event_name = "EVENT_BIND_FAILED";
				break;
			case ZMQ_EVENT_ACCEPTED:
				event_name = "EVENT_ACCEPTED";
				break;
			case ZMQ_EVENT_ACCEPT_FAILED:
				event_name = "EVENT_ACCEPT_FAILED";
				break;
			case ZMQ_EVENT_CLOSED:
				event_name = "EVENT_CLOSED";
				break;
			case ZMQ_EVENT_CLOSE_FAILED:
				event_name = "EVENT_CLOSE_FAILED";
				break;
			case ZMQ_EVENT_DISCONNECTED:
				event_name = "EVENT_DISCONNECTED";
				break;
			case ZMQ_EVENT_MONITOR_STOPPED:
				event_name = "EVENT_MONITOR_STOPPED";
				break;
			case ZMQ_EVENT_HANDSHAKE_FAILED_NO_DETAIL:
				event_name = "EVENT_HANDSHAKE_FAILED_NO_DETAIL";
				break;
			case ZMQ_EVENT_HANDSHAKE_SUCCEEDED:
				event_name = "EVENT_HANDSHAKE_SUCCEEDED";
				break;
			case ZMQ_EVENT_HANDSHAKE_FAILED_PROTOCOL:
				event_name = "EVENT_HANDSHAKE_FAILED_PROTOCOL";
				break;
			case ZMQ_EVENT_HANDSHAKE_FAILED_AUTH:
				event_name = "EVENT_HANDSHAKE_FAILED_AUTH";
				break;
			default:
				break;
		}
		if(log)
		{
			JLOG(INFO) << name_string.c_str() << " capture event " << event_name.c_str() << ", event: " << event << ", value: " << value << ", address: " << address.c_str();
		}
#if defined(_WIN32)
		Sleep(10);
#else
		usleep(1000 * 10);
#endif
	}

	// Close down the sockets
	zmq_close (monitor);
}

void * JwumqBase::ZmqSocket(void * ctx, ZMQ_TYPE_ENUM type)
{
	void * socket = NULL;
	if (type_kv.find(type) != type_kv.end() && ctx != NULL)
	{
		socket = zmq_socket (ctx, type_kv[type]);
	}
	return socket;
}

