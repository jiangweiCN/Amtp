//
//  jwumq_duplex_pub.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_duplex_pub.hpp"


JwumqDuplexPub::JwumqDuplexPub()
{
	loop = 0;
	conf_p = make_unique<DUPLEX_PUB_SUB_CONF_T>();
	this->recv_callback = nullptr;
	this->recv_callback_c = nullptr;
}

JwumqDuplexPub::~JwumqDuplexPub()
{
	loop = 0;
	conf_p.reset();
}

int JwumqDuplexPub::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}

int JwumqDuplexPub::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	this->recv_callback = callback;
	return Setup(setup_conf);
}

int JwumqDuplexPub::Setup(JWUMQ_SETUP_CONF_T * setup_conf)
{
	memcpy(conf_p.get(), setup_conf, sizeof(DUPLEX_PUB_SUB_CONF_T));
	string pub_identity = conf_p->identity;
	pub_identity = pub_identity + "_pub";
	memcpy(this->identity, pub_identity.c_str(), (int)pub_identity.length());
	this->identity[pub_identity.length()] = 0;
	JLOG(INFO) << "pub identity: " << identity;

	this->context = zmq_ctx_new ();
	if((pub_socket = zmq_socket (this->context, ZMQ_PUB)) == NULL)
    {
		JLOG(INFO) << "Setup socket error: " << strerror(errno);
        return LIB_JWUMQ_SOCKET_FAILD;
    }

	zmq_setsockopt(pub_socket, ZMQ_IDENTITY, identity, strlen(identity));
	if(zmq_bind(pub_socket, conf_p->pub_addr) != 0)
    {
		zmq_close(pub_socket);
		JLOG(INFO) << "Setup bind error: " << strerror(errno);
        return LIB_JWUMQ_BIND_FAILD;
    }
	JLOG(INFO) << "pub bind: " << conf_p->pub_addr;
	
	if((dealer_socket = zmq_socket (this->context, ZMQ_DEALER)) == NULL)
    {
        return LIB_JWUMQ_SOCKET_FAILD;
    }
	string dealer_identity = conf_p->identity;
	dealer_identity = dealer_identity + "_dealer";
	zmq_setsockopt(dealer_socket, ZMQ_IDENTITY, dealer_identity.c_str(), dealer_identity.length());
	if(zmq_bind(dealer_socket, conf_p->dealer_addr) < 0)
	{
		JLOG(INFO) << "zmq_bind error, errno: " << strerror(errno);
		zmq_close(dealer_socket);
		return LIB_JWUMQ_BIND_FAILD;
	}
	JLOG(INFO) << "dealer connect: " << conf_p->dealer_addr;

	loop = 1;
	recv_thread = thread(&JwumqDuplexPub::RecvThread, this);
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqDuplexPub::Release()
{
	loop = 0;
	zmq_close(pub_socket);
	zmq_close(dealer_socket);
	zmq_ctx_destroy(context);
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
}

int JwumqDuplexPub::Send(int command, char * from, char * to, void * data, int data_len)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}

int JwumqDuplexPub::Send(void * msg)
{
	if(pub_socket == NULL)
	{
		return LIB_JWUMQ_SOCKET_FAILD;
	}
	JwumqMessage *jmsg = (JwumqMessage*) msg;
	
	zmq_msg_t filter_msg;
	zmq_msg_init_size(&filter_msg, jmsg->body.des_id().length());
	memcpy (zmq_msg_data (&filter_msg), jmsg->body.des_id().c_str(), jmsg->body.des_id().length());
	
	zmq_msg_t send_msg;
	zmq_msg_init_size(&send_msg, jmsg->MsgDataLen());
	memcpy (zmq_msg_data (&send_msg), jmsg->MsgData(), jmsg->MsgDataLen());
	
	int rc = zmq_msg_send (&filter_msg, pub_socket, ZMQ_SNDMORE);
	zmq_msg_close (&filter_msg);
	
	int len = (int)zmq_msg_size(&send_msg);
	JLOG(INFO) << "Duplex pub send msg, length: " << len;
	
	rc = zmq_msg_send (&send_msg, pub_socket, 0);
	zmq_msg_close (&send_msg);

	if(rc < 0)
	{
		return LIB_JWUMQ_SEND_DATA_FAILD;
	}

	return LIB_JWUMQ_SUCCESS;
}

void JwumqDuplexPub::RecvThread()
{
	JLOG(INFO) << "Duplex pub recv thread start." ;
	
	zmq_pollitem_t items [] = { { dealer_socket, 0, ZMQ_POLLIN, 0 } };

	while (loop)
	{
		zmq_poll (items, 1, 5000);
		if (items [0].revents & ZMQ_POLLIN)
		{
			zmq_msg_t recv_msg;
			zmq_msg_init (&recv_msg);
			int recv_len = zmq_msg_recv (&recv_msg, dealer_socket, 0);
			if(recv_len <= 0)
			{
				zmq_msg_close (&recv_msg);
				usleep(1000);
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
//				recv_callback_c(static_cast<int>(jmsg->body.command()), jmsg->body.src_id, jmsg->RawData(), jmsg->RawDataLen());
			}
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	
	JLOG(INFO) << "Duplex pub recv thread exit." ;
}




