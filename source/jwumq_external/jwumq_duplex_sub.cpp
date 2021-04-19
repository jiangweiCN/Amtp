//
//  jwumq_duplex_sub.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_duplex_sub.hpp"



JwumqDuplexSub::JwumqDuplexSub()
{
	loop = 0;
	conf_p = make_unique<DUPLEX_PUB_SUB_CONF_T>();
	this->recv_callback = nullptr;
	this->recv_callback_c = nullptr;
}

JwumqDuplexSub::~JwumqDuplexSub()
{
	loop = 0;
	conf_p.reset();
}

int JwumqDuplexSub::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}

int JwumqDuplexSub::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	this->recv_callback = callback;
	return Setup(setup_conf);
}

int JwumqDuplexSub::Setup(JWUMQ_SETUP_CONF_T * setup_conf)
{
	memcpy(conf_p.get(), setup_conf, sizeof(DUPLEX_PUB_SUB_CONF_T));
	string sub_identity = conf_p->identity;
	sub_identity = sub_identity + "_sub";
	memcpy(this->identity, sub_identity.c_str(), (int)sub_identity.length());
	this->identity[sub_identity.length()] = 0;
	JLOG(INFO) << "sub identity: " << identity;

	this->context = zmq_ctx_new ();
	if((sub_socket = zmq_socket (this->context, ZMQ_SUB)) == NULL)
    {
		JLOG(INFO) << "Setup socket error: " << strerror(errno);
        return LIB_JWUMQ_SOCKET_FAILD;
    }

	zmq_setsockopt(sub_socket, ZMQ_IDENTITY, sub_identity.c_str(), sub_identity.length());
	zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, sub_identity.c_str(), sub_identity.length());
	zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "all", 3);

	if(zmq_connect(sub_socket, conf_p->pub_addr) < 0)
    {
        zmq_close(sub_socket);
		JLOG(INFO) << "Setup connect error: " << strerror(errno);
        return LIB_JWUMQ_CONNECT_FAILD;
    }
	JLOG(INFO) << "sub connect: " << conf_p->pub_addr;
	
	if((dealer_socket = zmq_socket (this->context, ZMQ_DEALER)) == NULL)
    {
        return LIB_JWUMQ_SOCKET_FAILD;
    }
	string dealer_identity = conf_p->identity;
	dealer_identity = dealer_identity + "_dealer";
	zmq_setsockopt(dealer_socket, ZMQ_IDENTITY, dealer_identity.c_str(), dealer_identity.length());
	if(zmq_connect(dealer_socket, conf_p->dealer_addr) < 0)
	{
		JLOG(INFO) << "zmq_connect error, errno: " << strerror(errno);
		zmq_close(dealer_socket);
		return LIB_JWUMQ_CONNECT_FAILD;
	}
	JLOG(INFO) << "dealer connect: " << conf_p->dealer_addr;
	
	loop = 1;
	recv_thread = thread(&JwumqDuplexSub::RecvThread, this);
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqDuplexSub::Release()
{
	loop = 0;
	zmq_close(sub_socket);
	zmq_ctx_destroy(context);
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
}

int JwumqDuplexSub::Send(int command, char * from, char * to, void * data, int data_len)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}

int JwumqDuplexSub::Send(void * msg)
{
	JwumqMessage *jmsg = (JwumqMessage*) msg;
	int result = 0;

	zmq_msg_t send_msg;
	zmq_msg_init_size(&send_msg, jmsg->MsgDataLen());
	memcpy(zmq_msg_data (&send_msg), jmsg->MsgData(), jmsg->MsgDataLen());
	
	result = zmq_msg_send (&send_msg, dealer_socket, 0);
	zmq_msg_close (&send_msg);
	if(result < 0)
	{
		return LIB_JWUMQ_SEND_DATA_FAILD;
	}
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqDuplexSub::RecvThread()
{
	JLOG(INFO) << "Duplex sub recv thread start." ;
	
	while (loop > 0)
	{
		int rc = 0;
		int64_t more;
		size_t more_size = sizeof more;
		
		zmq_msg_t filter_msg;
		zmq_msg_init (&filter_msg);
		rc = zmq_msg_recv (&filter_msg, sub_socket, 0);

		if(rc < 0)
		{
			zmq_msg_close (&filter_msg);
			continue;
		}
		char filter[64] = {0};
		sprintf(filter, "%s", (char*)zmq_msg_data(&filter_msg));
		filter[(int)zmq_msg_size(&filter_msg)] = 0;
		JLOG(INFO) << "Duplex sub recv filter(" << (int)zmq_msg_size(&filter_msg) << "): " << filter;
		zmq_msg_close (&filter_msg);
		
		zmq_getsockopt(sub_socket, ZMQ_RCVMORE, &more, &more_size);
		if(!more)
		{
			JLOG(ERROR) << "Duplex sub should recv more data, but only one recv.";
			continue;
		}
		zmq_msg_t recv_msg;
		zmq_msg_init (&recv_msg);
		rc = zmq_msg_recv (&recv_msg, sub_socket, 0);
		if(rc < 0)
		{
			JLOG(ERROR) << "Duplex sub recv error, " << rc;
			zmq_msg_close (&recv_msg);
			continue;
		}
		int len = (int)zmq_msg_size(&recv_msg);
		JLOG(INFO) << "Duplex sub recv msg, length: " << len;

		unique_ptr<JwumqMessage> jwumq_msg(new JwumqMessage((uint8_t *)zmq_msg_data (&recv_msg), len));
		zmq_msg_close (&recv_msg);

		if(recv_callback != nullptr)
		{
			recv_callback(jwumq_msg.get());
		}
		else if(recv_callback_c != nullptr)
		{
			recv_callback_c(static_cast<int>(jwumq_msg->body.command()), const_cast<char*>(jwumq_msg->body.src_id().c_str()),  jwumq_msg->RawData(), jwumq_msg->RawDataLen());
		}

	}
	
	JLOG(INFO) << "Duplex sub recv thread exit." ;
}






