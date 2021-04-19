//
//  jwumq_router_server.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_router_server.hpp"
#include "../jlog/jLog.h"


JwumqRouterServer::JwumqRouterServer()
{
	conf_p = make_unique<ROUTER_SERVER_CONF_T>();
	memset(identity, 0, sizeof(identity));
	loop = 0;
}

JwumqRouterServer::~JwumqRouterServer()
{
	conf_p = nullptr;
}

int JwumqRouterServer::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	memcpy(conf_p.get(), setup_conf, sizeof(ROUTER_SERVER_CONF_T));
	memcpy(this->identity, conf_p->identity, sizeof(this->identity));
	
	recv_callback = callback;
	this->context = zmq_ctx_new ();

	return Setup();
}
int JwumqRouterServer::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
void JwumqRouterServer::Release()
{
	loop = 0;
	
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	
	zmq_ctx_destroy(context);
}

int JwumqRouterServer::Setup()
{
	loop = 1;

	socket = zmq_socket(context, ZMQ_ROUTER);
	zmq_setsockopt(socket, ZMQ_IDENTITY, identity, strlen(identity));
	
	int const l = 0;
	zmq_setsockopt(socket, ZMQ_LINGER, &l, sizeof(l));
	zmq_bind (socket, conf_p->address);

	recv_thread = thread(&JwumqRouterServer::RecvThread, this);
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqRouterServer::RecvThread()
{
	JLOG(INFO) << "jwumq router server recv thread start." ;

	zmq_pollitem_t items [] = { { socket, 0, ZMQ_POLLIN, 0 } };
	while (loop)
	{
		zmq_poll (items, 1, 10000);
		if (items [0].revents & ZMQ_POLLIN)
		{
			int64_t more;
			size_t more_size = sizeof more;

			do{
				zmq_msg_t part;
				zmq_msg_init (&part);
				int recv_len = zmq_msg_recv (&part, socket, 0);
				if(recv_len <= -1)
				{
					JLOG(INFO) << "jwumq router server zmq_msg_recv error." ;
					break;
				}
				int rc = zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
				if(rc != 0)
				{
					JLOG(INFO) << "jwumq router server zmq_getsockopt error." ;
					break;
				}

				unique_ptr<char[]> buf(new char[recv_len + 1]());
				if(recv_len > 0)
				{
					memcpy(buf.get(), zmq_msg_data (&part), recv_len);
					buf[recv_len] = 0;
				}
				else
				{
					JLOG(INFO) << "jwumq router server recv_len <= 0." ;
					break;
				}

				if(more == 0)
				{
					unique_ptr<JwumqMessage> jmsg = make_unique<JwumqMessage>(buf.get(), recv_len);
					if(recv_callback != nullptr)
					{
						recv_callback(jmsg.get());
					}
					JLOG(INFO) << "jwumq router server recv_len: " << jmsg->RawDataLen();
				}
				else
				{
					JLOG(INFO) << "jwumq router server recv trace name: " << buf.get();
				}

				zmq_msg_close (&part);

			}while(more);
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	zmq_close(socket);
	JLOG(INFO) << "jwumq router server recv thread exit." ;
}

int JwumqRouterServer::Send(void * msg)
{
	JwumqMessage * jmsg = (JwumqMessage *)msg;
	
	int id_len = jmsg->body.des_id().length();
	zmq_msg_t id_msg;
	zmq_msg_init_size(&id_msg, id_len);
	memcpy(zmq_msg_data(&id_msg), jmsg->body.des_id().c_str(), id_len);
	int result = zmq_msg_send (&id_msg, socket, ZMQ_SNDMORE);
	zmq_msg_close (&id_msg);
	if(result < 0)
	{
		JLOG(ERROR) << "jwumq router server send error, id:" << jmsg->body.des_id().c_str();
		return LIB_JWUMQ_SEND_ID_FAILD;
	}
	
	zmq_msg_t send_msg;
	zmq_msg_init_size(&send_msg, jmsg->MsgDataLen());
	memcpy(zmq_msg_data(&send_msg), jmsg->MsgData(), jmsg->MsgDataLen());

	result = zmq_msg_send (&send_msg, socket, 0);
	zmq_msg_close (&send_msg);
	if(result < 0)
	{
		JLOG(ERROR) << "jwumq router server backend send error." ;
		return LIB_JWUMQ_SEND_DATA_FAILD;
	}
	
	return LIB_JWUMQ_SUCCESS;
}
int JwumqRouterServer::Send(int command, char * from, char * to, void * data, int data_len)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
