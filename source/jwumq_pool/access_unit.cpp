//
//  access_unit.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/22.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "access_unit.hpp"
#include <sys/time.h>
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"
//#include "factory_def.h"


AccessUnit::AccessUnit(string id, int port, shared_ptr<PoolStatus> status):pool_id(id),access_port(port), status(status)
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
	identity = pool_id + "_access";
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
			usleep(1000);
			continue;
		}

		shared_ptr<JwumqMessage> msg = nullptr;
		std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
		lock.lock();
		msg = send_q.front();
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
			usleep(1000);
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
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_pipe_req):
			{
				jwumq_pb_msg::CreatePipeQueueRequest req;
				req.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				string queue_name = req.queue_name();
				string node_id = req.node_id();
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_create_part_pipe_req, queue_name:"
				<< queue_name.c_str() << ", node_id: " << node_id.c_str();

				int result = 0;
				create_part_pipe_event(queue_name, node_id, result);
				
				jwumq_pb_msg::CreatePipeQueueResponse resp;
				resp.set_queue_name(queue_name);
				resp.set_node_id(node_id);
				resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E(result));

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_pipe_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_member_req):
			{
				jwumq_pb_msg::CreateMemberQueueRequest req;
				req.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				string queue_name = req.queue_name();
				string node_id = req.node_id();
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_create_part_member_req, queue_name:"
				<< queue_name.c_str() << ", node_id: " << node_id.c_str();

				int result = 0;
				create_part_member_event(queue_name, node_id, result);
				
				jwumq_pb_msg::CreateMemberQueueResponse resp;
				resp.set_queue_name(queue_name);
				resp.set_node_id(node_id);
				resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E(result));

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_member_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_producer_req):
			{
				jwumq_pb_msg::CreateProducerQueueRequest req;
				req.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				string queue_name = req.queue_name();
				string node_id = req.node_id();
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_create_part_producer_req, queue_name:"
				<< queue_name.c_str() << ", node_id: " << node_id.c_str();

				int result = 0;
				create_part_producer_event(queue_name, node_id, result);
				
				jwumq_pb_msg::CreateProducerQueueResponse resp;
				resp.set_queue_name(queue_name);
				resp.set_node_id(node_id);
				resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E(result));

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_producer_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_consumer_req):
			{
				jwumq_pb_msg::CreateConsumerQueueRequest req;
				req.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				string queue_name = req.queue_name();
				string node_id = req.node_id();
				JLOG(INFO) << "Recv " << recv_msg->body.src_id().c_str() << " private_create_part_consumer_req, queue_name:"
				<< queue_name.c_str() << ", node_id: " << node_id.c_str();

				int result = 0;
				create_part_consumer_event(queue_name, node_id, result);
				
				jwumq_pb_msg::CreateConsumerQueueResponse resp;
				resp.set_queue_name(queue_name);
				resp.set_node_id(node_id);
				resp.set_result(jwumq_pb_msg::JWUMQ_MSG_RESULT_E(result));

				size_t resp_len = resp.ByteSizeLong();
				unique_ptr<char[]> resp_buf(new char[resp_len]());
				resp.SerializeToArray(resp_buf.get(), resp_len);
				
				shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_consumer_resp, identity, recv_msg->body.src_id(),  resp_buf.get(), (int)resp_len));
				std::unique_lock<std::mutex> lock(send_mutex);
				send_q.push(send_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_text):
			{
				int len = recv_msg->RawDataLen();
				JLOG(INFO) << "Recv public text message, queue name: " << recv_msg->body.queue_name().c_str() << ", len: " << len << ", src_id: " << recv_msg->body.src_id().c_str() << ", des_id: " << recv_msg->body.des_id().c_str();
				recv_public_text_event(recv_msg);
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_bytes):
			{
				int len = recv_msg->RawDataLen();
				JLOG(INFO) << "Recv public bytes message, queue name: " << recv_msg->body.queue_name().c_str() << ", len: " << len << ", src_id: " << recv_msg->body.src_id().c_str() << ", des_id: " << recv_msg->body.des_id().c_str();
				recv_public_bytes_event(recv_msg);
				break;
			}
			default:break;
		}
	}
	JLOG(INFO) << "Access unit recv thread end! ";
}

int AccessUnit::Send(shared_ptr<JwumqMessage> send_msg)
{
	std::unique_lock<std::mutex> lock(send_mutex);
	send_q.push(send_msg);
	return JWUMQ_SUCCESS;
}
