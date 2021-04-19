//
//  jwumq_proxy.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_proxy.hpp"

JwumqProxy::JwumqProxy()
{
	conf_p = make_unique<PROXY_CONF_T>();
	memset(identity, 0, sizeof(identity));
	loop = 0;
}

JwumqProxy::~JwumqProxy()
{
	conf_p = nullptr;
}

int JwumqProxy::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	memcpy(conf_p.get(), setup_conf, sizeof(PROXY_CONF_T));
	memcpy(this->identity, conf_p->identity, sizeof(this->identity));
	
	recv_callback = callback;
	this->context = zmq_ctx_new ();

	return Setup();
}
int JwumqProxy::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
void JwumqProxy::Release()
{
	loop = 0;
	
	if(frontend_monitor_thread.joinable())
	{
		frontend_monitor_thread.join();
	}
	
	if(backend_monitor_thread.joinable())
	{
		backend_monitor_thread.join();
	}
	
	zmq_ctx_destroy(context);
}

int JwumqProxy::Setup()
{
	proxy_thread = thread(&JwumqProxy::ProxyThread, this);
	return LIB_JWUMQ_SUCCESS;
}

void JwumqProxy::ProxyThread()
{
	int major, minor, patch;
	zmq_version (&major, &minor, &patch);
	JLOG(INFO) << "jwumq proxy thread start. V:" << major << "." << minor << "." << patch;
	
	char front_identity [MAX_IDENTITY_BUF_SIZE + 32] = {0};
	char back_identity [MAX_IDENTITY_BUF_SIZE + 32] = {0};
    sprintf (front_identity, "%s_frontend", identity);
    sprintf (back_identity, "%s_backend", identity);
	char back_monitor_addr [MAX_ADDRESS_BUF_SIZE + 32] = {0};
	char front_monitor_addr [MAX_ADDRESS_BUF_SIZE + 32] = {0};
	sprintf (front_monitor_addr, "inproc://%s_frontend_monitor", identity);
    sprintf (back_monitor_addr, "inproc://%s_backend_monitor", identity);
	
	loop = 1;
	
	if((frontend = ZmqSocket(context, conf_p->frontend_type)) == NULL)
	{
		JLOG(INFO) << "jwumq proxy thread exit, because frontend type error." ;
		return ;
	}
	
	if(zmq_socket_monitor(frontend, front_monitor_addr, ZMQ_EVENT_ALL) < 0)
	{
		JLOG(ERROR) << "Frontend socket monitor error " << strerror(errno);
	}
	else
	{
		frontend_monitor_thread = thread(&JwumqProxy::SocketMonitorThread, this, front_identity, context, frontend, front_monitor_addr, true);
	}
	
	if((backend = ZmqSocket(context, conf_p->backend_type)) == NULL)
	{
		JLOG(INFO) << "jwumq proxy thread exit, because backend type error." ;
		return;
	}
	
	if(zmq_socket_monitor(backend, back_monitor_addr, ZMQ_EVENT_ALL) < 0)
	{
		JLOG(ERROR) << "Backend socket monitor error " << strerror(errno);
	}
	else
	{
		backend_monitor_thread = thread(&JwumqProxy::SocketMonitorThread, this, back_identity, context, backend, back_monitor_addr, true);
	}
	
	zmq_setsockopt(frontend, ZMQ_IDENTITY, front_identity, strlen(front_identity));
	zmq_setsockopt(backend, ZMQ_IDENTITY, back_identity, strlen(back_identity));
	int const l = 0;
	zmq_setsockopt(frontend, ZMQ_LINGER, &l, sizeof(l));
	zmq_setsockopt(backend, ZMQ_LINGER, &l, sizeof(l));
	
	if(conf_p->frontend_bind)
	{
		zmq_bind (frontend, conf_p->frontend_addr);
		JLOG(INFO) << "jwumq proxy frontend bind " << conf_p->frontend_addr;
	}
	else
	{
		zmq_connect (frontend, conf_p->frontend_addr);
		JLOG(INFO) << "jwumq proxy frontend connect " << conf_p->frontend_addr;
	}
	if(conf_p->backend_bind)
	{
		zmq_bind (backend,  conf_p->backend_addr);
		JLOG(INFO) << "jwumq proxy backend bind " << conf_p->backend_addr;
	}
	else
	{
		zmq_connect (backend,  conf_p->backend_addr);
		JLOG(INFO) << "jwumq proxy backend connect " << conf_p->backend_addr;
	}
	
	zmq_proxy (frontend, backend, NULL);
	
	zmq_close(frontend);
	zmq_close(backend);

	JLOG(INFO) << "jwumq proxy thread exit." ;
}

int JwumqProxy::Send(void * msg)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
int JwumqProxy::Send(int command, char * from, char * to, void * data, int data_len)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
