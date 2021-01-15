//
//  access_unit.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "access_unit.hpp"
#include <sys/time.h>
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"
#include "factory_def.h"


AccessUnit::AccessUnit(string id, int access_port):factory_id(id), access_port(access_port)
{
	conn = make_unique<JwumqRouterServer>();
}

AccessUnit::~AccessUnit()
{
	loop = false;
}

bool AccessUnit::Start()
{
	JLOG(INFO) << "Access unit start.";
	
	int result = Setup();
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return false;
	}
	
	loop = true;
	send_thread = thread(&AccessUnit::SendThread, this);
	recv_thread = thread(&AccessUnit::RecvThread, this);

	return true;
}

bool AccessUnit::End()
{
	loop = false;
	conn->Release();
	if(send_thread.joinable())
	{
		send_thread.join();
	}
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	queue<shared_ptr<JwumqMessage>> empty_s;
	swap(empty_s, send_q);

	queue<shared_ptr<JwumqMessage>> empty_r;
	swap(empty_r, recv_q);
	return true;
}

int AccessUnit::Setup()
{
	identity = factory_id + "_access";
	string address = "tcp://*:" + to_string(access_port);


	ROUTER_SERVER_CONF_T conf_t;
	memcpy(conf_t.address, address.c_str(), address.length());
	conf_t.address[address.length()] = 0;
	memcpy(conf_t.identity, identity.c_str(), identity.length());
	conf_t.identity[identity.length()] = 0;
	conf_t.linger = 1;
	conf_t.read_timeout = 5000;
	conf_t.bind = true;

	return conn->Setup(&conf_t, std::bind(&AccessUnit::RecvCallback, this
	, std::placeholders::_1));
}

int AccessUnit::RecvCallback(void * recv_msg)
{
	JLOG(INFO) << "Recv access msg! ";
	shared_ptr<JwumqMessage> msg(new JwumqMessage((JwumqMessage *)recv_msg));
	std::unique_lock<std::mutex> lock(recv_mutex);
	recv_q.push(msg);
	return 0;
}

void AccessUnit::SendThread()
{
	JLOG(INFO) << "Access unit send thread begin! ";
	while (loop)
	{
		if(send_q.size() <= 0)
		{
			usleep(1000 * 10);
			continue;
		}

		std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
		lock.lock();
		shared_ptr<JwumqMessage> msg = send_q.front();
		send_q.pop();
		lock.unlock();

		if(msg != nullptr)
		{
			int result = conn->Send(msg.get());
			JLOG(INFO) << "Send access unit msg, cmd: " << static_cast<uint32_t>(msg->body.command())
						<< ", result: " << result;
		}
	}
	JLOG(INFO) << "Access unit send thread end! ";
}

void AccessUnit::RecvThread()
{
	JLOG(INFO) << "Access unit recv thread begin! ";
	while (loop)
	{
		if(recv_q.size() <= 0)
		{
			usleep(1000 * 10);
			continue;
		}

		std::unique_lock<std::mutex> lock(recv_mutex, std::defer_lock);
		if (!lock.try_lock())
		{
			usleep(1000);
			continue;
		}
		std::mutex *ptx = lock.release();
		shared_ptr<JwumqMessage> recv_msg = recv_q.front();
		recv_q.pop();
		ptx->unlock();

		switch(recv_msg->body.command())
		{
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_req):
			{
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_alive_req.";
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_resp, identity, recv_msg->body.src_id(),  recv_msg->RawData(), recv_msg->RawDataLen()));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_session_req):
			{
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_session_req.";
				
				struct timeval tv;
				gettimeofday(&tv,NULL);
				uint64_t session = tv.tv_sec*1000000 + tv.tv_usec;
				
				jwumq_pb_msg::SessionResponse resp;
				resp.set_session(session);

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_session_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_queue_req):
			{
				jwumq_pb_msg::CreateQueueRequest req;
				req.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				string queue_name = req.queue_name();
				uint64_t session = req.session();
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_create_queue_req, queue_name:"
							<< queue_name.c_str() << ", session: " << session;
				
				jwumq_pb_msg::CreateQueueResponse resp;
				resp.set_session(session);
				
				string pool_name;
				int access_port;
				get_available_pool_fun(queue_name, pool_name, access_port);
				if(access_port <= 0)
				{
					resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E::CREATE_QUEUE_ERROR);
					JLOG(INFO) << "No available queue pool for " << queue_name.c_str();
				}
				else
				{
					
					resp.set_queue_name(queue_name);
					resp.set_queue_port(access_port);
					resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS);
					JLOG(INFO) << "Assigned " << queue_name.c_str() << " to "<< pool_name.c_str() << ", access port: " << access_port;
					assigned_queue(pool_name, session, queue_name);
				}
				

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_queue_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			default:break;
		}
	}
	JLOG(INFO) << "Access unit recv thread end! ";
}

