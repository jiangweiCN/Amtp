//
//  jwumq_router.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_router.hpp"


JwumqRouter::JwumqRouter()
{
	conf_p = make_unique<ROUTER_CONF_T>();
	memset(identity, 0, MAX_IDENTITY_BUF_SIZE);
	loop = 0;
}

JwumqRouter::~JwumqRouter()
{
	conf_p = nullptr;
}

int JwumqRouter::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	memcpy(conf_p.get(), setup_conf, sizeof(ROUTER_CONF_T));
	memcpy(this->identity, conf_p->identity, sizeof(this->identity));
	
	recv_callback = callback;
	this->context = zmq_ctx_new ();

	return Setup();
}
int JwumqRouter::Setup(JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
void JwumqRouter::Release()
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

int JwumqRouter::Setup()
{
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
		JLOG(INFO) << "jwumq router thread exit, because frontend type error." ;
		return LIB_JWUMQ_TYPE_ABNORMAL;
	}
	
	if(zmq_socket_monitor(frontend, front_monitor_addr, ZMQ_EVENT_ALL) < 0)
	{
		JLOG(ERROR) << "Frontend socket monitor error " << strerror(errno);
	}
	else
	{
		frontend_monitor_thread = thread(&JwumqRouter::SocketMonitorThread, this, front_identity, context, frontend, front_monitor_addr, true);
	}
	
	if((backend = ZmqSocket(context, conf_p->backend_type)) == NULL)
	{
		JLOG(INFO) << "jwumq router thread exit, because backend type error." ;
		return LIB_JWUMQ_TYPE_ABNORMAL;
	}
	
	if(zmq_socket_monitor(backend, back_monitor_addr, ZMQ_EVENT_ALL) < 0)
	{
		JLOG(ERROR) << "Backend socket monitor error " << strerror(errno);
	}
	else
	{
		backend_monitor_thread = thread(&JwumqRouter::SocketMonitorThread, this, back_identity, context, backend, back_monitor_addr, true);
	}
	
	zmq_setsockopt(frontend, ZMQ_IDENTITY, front_identity, strlen(front_identity));
	zmq_setsockopt(backend, ZMQ_IDENTITY, back_identity, strlen(back_identity));
	int const l = 0;
	zmq_setsockopt(frontend, ZMQ_LINGER, &l, sizeof(l));
	zmq_setsockopt(backend, ZMQ_LINGER, &l, sizeof(l));
	
	if(conf_p->frontend_bind)
	{
		zmq_bind (frontend, conf_p->frontend_addr);
		JLOG(INFO) << "jwumq router frontend bind " << conf_p->frontend_addr;
	}
	else
	{
		zmq_connect (frontend, conf_p->frontend_addr);
		JLOG(INFO) << "jwumq router frontend connect " << conf_p->frontend_addr;
	}
	if(conf_p->backend_bind)
	{
		zmq_bind (backend,  conf_p->backend_addr);
		JLOG(INFO) << "jwumq router backend bind " << conf_p->backend_addr;
	}
	else
	{
		zmq_connect (backend,  conf_p->backend_addr);
		JLOG(INFO) << "jwumq router backend connect " << conf_p->backend_addr;
	}
	
	
	router_table.clear();
	frontend_recv_thread = thread(&JwumqRouter::FrontendRecvThread, this);
	backend_recv_thread = thread(&JwumqRouter::BackendRecvThread, this);
	
	return LIB_JWUMQ_SUCCESS;
}

void JwumqRouter::FrontendRecvThread()
{
	JLOG(INFO) << "jwumq router frontend recv thread start." ;
	
	zmq_pollitem_t items [] = { { frontend, 0, ZMQ_POLLIN, 0 } };
	while (loop)
	{
		zmq_poll (items, 1, 10000);
		if (items [0].revents & ZMQ_POLLIN)
		{
			int64_t more;
			size_t more_size = sizeof more;
			TRACE_LIST trace_list;
			do{
				zmq_msg_t part;
				zmq_msg_init (&part);
				int recv_len = zmq_msg_recv (&part, frontend, 0);
				if(recv_len <= -1)
				{
					JLOG(INFO) << "jwumq router frontend zmq_msg_recv error." ;
					break;
				}
				int rc = zmq_getsockopt (frontend, ZMQ_RCVMORE, &more, &more_size);
				if(rc != 0)
				{
					JLOG(INFO) << "jwumq router frontend zmq_getsockopt error." ;
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
					JLOG(INFO) << "jwumq router frontend recv_len <= 0." ;
					break;
				}
				
				if(more == 0)
				{
					string des_id = trace_list.back();
					
					unique_ptr<JwumqMessage> jmsg = make_unique<JwumqMessage>(buf.get(), recv_len);
					if(jmsg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_data))
					{
						UpdateRouterTable(trace_list);
					}
					else if (router_table.find(des_id) == router_table.end())
					{
						UpdateRouterTable(trace_list);
					}
					
					zmq_msg_t send_msg;
					zmq_msg_init_size(&send_msg, jmsg->MsgDataLen());
					memcpy(zmq_msg_data (&send_msg), jmsg->MsgData(), jmsg->MsgDataLen());
					
					int result = zmq_msg_send (&send_msg, backend, 0);
					zmq_msg_close (&send_msg);
					if(result < 0)
					{
						JLOG(ERROR) << "jwumq router backend send error." ;
						return ;
					}
				}
				else
				{
					trace_list.push_back(buf.get());
//					JLOG(INFO) << "jwumq router frontend recv trace name: " << buf.get();
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
	zmq_close(frontend);
	JLOG(INFO) << "jwumq router frontend recv thread exit." ;
}

void JwumqRouter::UpdateRouterTable(TRACE_LIST trace)
{
	std::unique_lock<std::mutex> lock(router_table_mutex);
	if(trace.size() <= 0)
	{
		JLOG(ERROR) << "jwumq router update router table error. because trace info list is null.";
		return ;
	}
	string des_id = trace.back();
	if(des_id.length() == 0)
	{
		JLOG(ERROR) << "jwumq router update router table error. des_id is null.";
		return ;
	}
	JLOG(INFO) << "jwumq router update router table. trace des_id: " << des_id.c_str() ;
	router_table[des_id] = trace;
	
	PrintRouterTable(des_id);
}
void JwumqRouter::PrintRouterTable()
{
	for(auto &table : router_table)
	{
		string trace;
		for(auto &node : table.second)
		{
			trace += node + ",";
		}
		
		JLOG(INFO) << "jwumq router table. des_id: " << table.first.c_str() << ", trace info: " << trace;
	}
}
void JwumqRouter::PrintRouterTable(string des_id)
{
	TRACE_LIST tl = router_table[des_id];
	string trace;
	for(auto &node : tl)
	{
		trace += node + ",";
	}
	
	JLOG(INFO) << "jwumq router table. des_id: " << des_id << ", trace info: " << trace;
}
TRACE_LIST JwumqRouter::GetTraceInfo(string des_id)
{
	std::unique_lock<std::mutex> lock(router_table_mutex);
	TRACE_LIST trace;
	if (router_table.find(des_id) != router_table.end())
	{
		trace = router_table[des_id];
	}
	return trace;
}
void JwumqRouter::BackendRecvThread()
{
	JLOG(INFO) << "jwumq router backend recv thread start." ;
	zmq_pollitem_t items [] = { { backend, 0, ZMQ_POLLIN, 0 } };
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
				int recv_len = zmq_msg_recv (&part, backend, 0);
				if(recv_len <= -1)
				{
					JLOG(INFO) << "jwumq router backend zmq_msg_recv error." ;
					break;
				}
				int rc = zmq_getsockopt (backend, ZMQ_RCVMORE, &more, &more_size);
				if(rc != 0)
				{
					JLOG(INFO) << "jwumq router backend zmq_getsockopt error." ;
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
					JLOG(INFO) << "jwumq router frontend recv_len <= 0." ;
					break;
				}
				
				if(more == 0)
				{
					unique_ptr<JwumqMessage> jmsg = make_unique<JwumqMessage>(buf.get(), recv_len);
					
					TRACE_LIST trace_list = GetTraceInfo(jmsg->body.des_id());
					if(trace_list.size() < 1)
					{
						JLOG(ERROR) << "jwumq router backend send error, trace list is null. des_id:" << jmsg->body.des_id().c_str() ;
						break;
					}
					for(auto &trace : trace_list)
					{
						zmq_msg_t id_msg;
						zmq_msg_init_size(&id_msg, trace.length());
						memcpy(zmq_msg_data (&id_msg), trace.c_str(), trace.length());
						int result = zmq_msg_send (&id_msg, frontend, ZMQ_SNDMORE);
						zmq_msg_close (&id_msg);
						if(result < 0)
						{
							JLOG(ERROR) << "jwumq router backend send error, trace:" << trace.c_str();
							break;
						}
					}
					
					zmq_msg_t send_msg;
					zmq_msg_init_size(&send_msg, recv_len);
					memcpy(zmq_msg_data (&send_msg), buf.get(), recv_len);

					int result = zmq_msg_send (&send_msg, frontend, 0);
					zmq_msg_close (&send_msg);
					if(result < 0)
					{
						JLOG(ERROR) << "jwumq router backend send error." ;
						return ;
					}
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
	zmq_close(backend);
	JLOG(INFO) << "jwumq router backend recv thread exit." ;
}


int JwumqRouter::Send(void * msg)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
int JwumqRouter::Send(int command, char * from, char * to, void * data, int data_len)
{
	return LIB_JWUMQ_FUN_UNAVAILABLE;
}
