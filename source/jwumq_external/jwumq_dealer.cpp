//
//  jwumq_dealer.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_dealer.hpp"


JwumqDealer::JwumqDealer()
{
	loop = 0;
	conf_p = make_unique<DEALER_CONF_T>();
	this->recv_callback = nullptr;
	this->recv_callback_c = nullptr;
	memset(identity, 0, sizeof(identity));
}
 
JwumqDealer::~JwumqDealer()
{
	loop = 0;
	conf_p = nullptr;
}
int JwumqDealer::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	this->recv_callback_c = callback;
	return Setup(setup_conf);
}
int JwumqDealer::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	this->recv_callback = callback;
	return Setup(setup_conf);
}
int JwumqDealer::Setup(JWUMQ_SETUP_CONF_T * setup_conf)
{
	memcpy(conf_p.get(), setup_conf, sizeof(DEALER_CONF_T));
	memcpy(this->identity, conf_p->identity, strlen(conf_p->identity));
	
	
	this->context = zmq_ctx_new ();

	if((socket = zmq_socket (this->context, ZMQ_DEALER)) == NULL)
    {
        return LIB_JWUMQ_SOCKET_FAILD;
    }
//	int const linger = conf_p->linger;
//	zmq_setsockopt(this->socket, ZMQ_LINGER, &linger, sizeof(linger));
	zmq_setsockopt(this->socket, ZMQ_IDENTITY, identity, strlen(identity));
#if defined(_WIN32)
	int timeout = 1000;
	zmq_setsockopt(this->socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
#else
#endif
	
//	char monitor_addr [MAX_ADDRESS_BUF_SIZE + 32] = {0};
//	sprintf (monitor_addr, "inproc://%s_monitor", identity);
//	if(zmq_socket_monitor(socket, monitor_addr, ZMQ_EVENT_ALL) < 0)
//	{
//		JLOG(ERROR) << "Socket monitor error " << strerror(errno);
//	}
//	else
//	{
//		monitor_thread = thread(&JwumqDealer::SocketMonitorThread, this, identity, context, this->socket, monitor_addr, true);
//	}

	if(conf_p->bind)
	{
		int res = 0;
		if((res = zmq_bind(this->socket, conf_p->address)) < 0)
		{
#if defined(_WIN32)
			char errmsg[128];
			strerror_s(errmsg, 128, errno);
			JLOG(INFO) << "zmq_bind error: " << res << ", cause: " << errmsg;
#else
			JLOG(INFO) << "zmq_bind error: " << res << ", cause: " << strerror(errno);
#endif
			zmq_close(socket);
			return LIB_JWUMQ_BIND_FAILD;
		}
	}
	else
	{
		if(zmq_connect(this->socket, conf_p->address) < 0)
		{
			zmq_close(socket);
			return LIB_JWUMQ_CONNECT_FAILD;
		}
	}
	loop = 1;
	recv_thread = thread(&JwumqDealer::RecvThread, this);
	return LIB_JWUMQ_SUCCESS;
}
void JwumqDealer::Release()
{
	loop = 0;
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	zmq_close(socket);
	/*
	* windows 下，销毁一个上下文正常，销毁2个不同上下文则程序异常，原因待查。Linux无此问题
	*/
#if defined(_WIN32)
#else
	zmq_ctx_destroy(context);
#endif
	
	
}
int JwumqDealer::Send(int command, char * from, char * to, void * data, int data_len)
{
	unique_ptr<JwumqMessage> msg(new JwumqMessage(static_cast<JWUMQ_COMMAND_ENUM>(command), from, to, data, data_len));
	return Send(msg.get());
}
int JwumqDealer::Send(void * msg)
{
	JwumqMessage *jmsg = (JwumqMessage*) msg;
	int result = 0;

	zmq_msg_t send_msg;
	zmq_msg_init_size(&send_msg, jmsg->MsgDataLen());
	memcpy(zmq_msg_data (&send_msg), jmsg->MsgData(), jmsg->MsgDataLen());
	//JLOG(INFO) << "Debug JwumqDealer::Send, " << "len:" << jmsg->MsgDataLen();
	result = zmq_msg_send (&send_msg, socket, ZMQ_DONTWAIT);
	//JLOG(INFO) << "Debug JwumqDealer::Send finish";
	zmq_msg_close (&send_msg);
	if(result < 0)
	{
		return LIB_JWUMQ_SEND_DATA_FAILD;
	}
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqDealer::RecvThread()
{
	zmq_pollitem_t items [] = { { socket, 0, ZMQ_POLLIN, 0 } };

	while (loop)
	{
		zmq_poll (items, 1, conf_p->read_timeout);
		if (items [0].revents & ZMQ_POLLIN)
		{
			zmq_msg_t recv_msg;
			zmq_msg_init (&recv_msg);
			int recv_len = zmq_msg_recv (&recv_msg, socket, 0);
			if(recv_len <= 0)
			{
				zmq_msg_close (&recv_msg);
#if defined(_WIN32)
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			}
			
			unique_ptr<JwumqMessage> jmsg = make_unique<JwumqMessage>(zmq_msg_data (&recv_msg), recv_len);
			zmq_msg_close (&recv_msg);
			
			if(recv_callback != NULL)
			{
				recv_callback(jmsg.get());
			}
			else if(recv_callback_c != NULL)
			{
				recv_callback_c(static_cast<int>(jmsg->body.command()), const_cast<char*>(jmsg->body.src_id().c_str()), jmsg->RawData(), jmsg->RawDataLen());
			}
		}
		else
		{
#if defined(_WIN32)
			Sleep(1);
#else
			usleep(1000);
#endif
			continue;
		}
	}
	//zmq_close(socket);
}
