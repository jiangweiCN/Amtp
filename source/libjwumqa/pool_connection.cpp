//
//  pool_connection.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "pool_connection.hpp"
#include <unistd.h>
#include "../jwumq_protocol/jwumq_msg.pb.h"
#include "JTextMessageImpl.hpp"
#include "JBytesMessageImpl.hpp"


PoolConnection::PoolConnection(string url, string queue_name, string node_id):url(url), queue_name(queue_name), identity(node_id)
{
	JLOG(INFO) << "PoolConnection::PoolConnection";
	conn = make_unique<JwumqDealer>();
}

PoolConnection::~PoolConnection()
{
	JLOG(INFO) << "PoolConnection::~PoolConnection";
}

int PoolConnection::Connect()
{
	JLOG(INFO) << "PoolConnection::Connect, url: " << url.c_str() << ", queue name: " << queue_name.c_str() << ", id: " << identity.c_str();

	int result = Setup();
	JLOG(INFO) << "PoolConnection::Connect, result: " << result;
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}

	loop = true;
	send_thread = thread(&PoolConnection::SendThread, this);
	alive_thread = thread(&PoolConnection::AliveThread, this);

	return JWUMQ_SUCCESS;
}

void PoolConnection::Close()
{
	JLOG(INFO) << "PoolConnection::Close";
	loop = false;
	if(send_thread.joinable())
	{
		send_thread.join();
	}
	if(alive_thread.joinable())
	{
		alive_thread.join();
	}
	conn->Release();
}

int PoolConnection::Setup()
{
	DEALER_CONF_T conf_t;
	memcpy(conf_t.address, url.c_str(), url.length());
	conf_t.address[url.length()] = 0;
	memcpy(conf_t.identity, identity.c_str(), identity.length());
	conf_t.identity[identity.length()] = 0;
	conf_t.linger = 1;
	conf_t.read_timeout = 5000;
	conf_t.bind = false;

	return conn->Setup(&conf_t, std::bind(&PoolConnection::RecvCallback, this
	, std::placeholders::_1));
}

int PoolConnection::CreatePartPipe(int timeout)
{
	jwumq_pb_msg::CreatePipeQueueRequest req;
	req.set_queue_name(queue_name);
	req.set_node_id(identity);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);

	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_pipe_req, identity, "",  req_buf.get(), (int)req_len));
	std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "PoolConnection push create part pipe request!";

	std::unique_lock<std::mutex> ack_lock(req_part_pipe_mutex);
	if(req_part_pipe_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	return JWUMQ_SUCCESS;
}

int PoolConnection::CreatePartMember(int timeout)
{
	jwumq_pb_msg::CreateMemberQueueRequest req;
	req.set_queue_name(queue_name);
	req.set_node_id(identity);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);

	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_member_req, identity, "",  req_buf.get(), (int)req_len));
	std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "PoolConnection push create part member request!";

	std::unique_lock<std::mutex> ack_lock(req_part_member_mutex);
	if(req_part_member_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	return JWUMQ_SUCCESS;
}


int PoolConnection::CreatePartProducer(int timeout)
{
	jwumq_pb_msg::CreateProducerQueueRequest req;
	req.set_queue_name(queue_name);
	req.set_node_id(identity);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);

	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_producer_req, identity, "",  req_buf.get(), (int)req_len));
	std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "PoolConnection push create part producer request!";

	std::unique_lock<std::mutex> ack_lock(req_part_producer_mutex);
	if(req_part_producer_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	return JWUMQ_SUCCESS;
}

int PoolConnection::CreatePartConsumer(int timeout)
{
	jwumq_pb_msg::CreateConsumerQueueRequest req;
	req.set_queue_name(queue_name);
	req.set_node_id(identity);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);

	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_part_consumer_req, identity, "",  req_buf.get(), (int)req_len));
	std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "PoolConnection push create part consumer request!";

	std::unique_lock<std::mutex> ack_lock(req_part_consumer_mutex);
	if(req_part_consumer_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	return JWUMQ_SUCCESS;
}

void PoolConnection::AliveThread()
{
	time_t l_t = 0, c_t = 0;
	while (loop)
	{
		c_t = time(NULL);
		if(c_t - l_t >= 10)
		{
			l_t = c_t;
			if(send_q.size() <= 0)
			{
				RequestAlive(3000);
			}
		}
		usleep(1000*10);
	}
}

int PoolConnection::RequestAlive(int timeout)
{
	string alive = identity + ", alive";
	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, identity, "",  alive.c_str(), (int)alive.length()));
	std::unique_lock<std::mutex> lock(send_mutex);
	send_q.push(msg);
	JLOG(INFO) << "PoolConnection push alive!";
	return JWUMQ_SUCCESS;
}

void PoolConnection::SendThread()
{
	while (loop)
	{
		if(send_q.size() <= 0)
		{
			usleep(1000);
			continue;
		}
		std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
		if (!lock.try_lock())
		{
			usleep(1000);
			continue;
		}
		std::mutex *ptx = lock.release();
		shared_ptr<JwumqMessage> msg = send_q.front();
		send_q.pop();
		ptx->unlock();
		conn->Send(msg.get());
	}
}

int PoolConnection::RecvCallback(void * recv_msg)
{
	JLOG(INFO) << "PoolConnection::RecvCallback";
	shared_ptr<JwumqMessage> msg(new JwumqMessage((JwumqMessage *)recv_msg));
	
	if(msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_pipe_resp))
	{
		jwumq_pb_msg::CreatePipeQueueResponse resp;
		resp.ParseFromArray(msg->RawData(), msg->RawDataLen());

		uint32_t result = resp.result();
		JLOG(INFO) << "PoolConnection recv private_create_part_pipe_resp, result: " << result;
		create_part_pipe_resp_event(result);
		req_part_pipe_cv.notify_all();
	}
	else if(msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_member_resp))
	{
		jwumq_pb_msg::CreateMemberQueueResponse resp;
		resp.ParseFromArray(msg->RawData(), msg->RawDataLen());

		uint32_t result = resp.result();
		JLOG(INFO) << "PoolConnection recv private_create_part_member_resp, result: " << result;
		create_part_member_resp_event(result);
		req_part_member_cv.notify_all();
	}
	else if(msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_producer_resp))
	{
		jwumq_pb_msg::CreateProducerQueueResponse resp;
		resp.ParseFromArray(msg->RawData(), msg->RawDataLen());

		uint32_t result = resp.result();
		JLOG(INFO) << "PoolConnection recv private_create_part_producer_resp, result: " << result;
		create_part_producer_resp_event(result);
		req_part_producer_cv.notify_all();
	}
	else if(msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_part_consumer_resp))
	{
		jwumq_pb_msg::CreateConsumerQueueResponse resp;
		resp.ParseFromArray(msg->RawData(), msg->RawDataLen());

		uint32_t result = resp.result();
		JLOG(INFO) << "PoolConnection recv private_create_part_consumer_resp, result: " << result;
		create_part_consumer_resp_event(result);
		req_part_consumer_cv.notify_all();
	}
	else if(msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_text)
			|| msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_bytes))
	{
		std::unique_lock<std::mutex> lock(recv_mutex);
		recv_q.push(msg);
		if(recv_q.size() > 100)
		{
			recv_q.pop();
		}
	}
	
	return 0;
}

int PoolConnection::Send(JMessage * message, int type)
{
	if(message == nullptr)
	{
		JLOG(INFO) << "JMessage is nullptr";
		return JWUMQ_MESSAGE_IS_NULL;
	}
	if(dynamic_cast<JTextMessageImpl*>(message))
	{
		JTextMessageImpl * msg = (JTextMessageImpl *)message;
		shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_text, identity, msg->GetDestination(), queue_name,  msg->GetText().c_str(), (int)msg->GetText().length()));
		std::unique_lock<std::mutex> lock(send_mutex);
		send_q.push(send_msg);
		return JWUMQ_SUCCESS;
	}
	else if(dynamic_cast<JBytesMessageImpl*>(message))
	{
		JBytesMessageImpl * msg = (JBytesMessageImpl *)message;
		int length = msg->GetLength();
		shared_ptr<JwumqMessage> send_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_bytes, identity, msg->GetDestination(), queue_name,  msg->GetBytes(), length));
		std::unique_lock<std::mutex> lock(send_mutex);
		send_q.push(send_msg);
		return JWUMQ_SUCCESS;
	}
	return JWUMQ_MESSAGE_UNKNOWN;
}

JMessage * PoolConnection::Receive(int timeout)
{
	JMessage * message = nullptr;
	int index = timeout;
	while(index > 0)
	{
		if(recv_q.size() > 0)
		{
			std::unique_lock<std::mutex> lock(recv_mutex, std::defer_lock);
			lock.lock();
			shared_ptr<JwumqMessage> recv_msg = recv_q.front();
			recv_q.pop();
			lock.unlock();
			
			if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_text))
			{
				JLOG(INFO) << "PoolConnection recv public_data_text, src_id: " << recv_msg->body.src_id();
				
				int len = recv_msg->RawDataLen();
				unique_ptr<char[]> buf(new char[len + 1]());
				memcpy(buf.get(), recv_msg->RawData(), len);
				buf[len] = 0;
				message = new JTextMessageImpl(buf.get());
				message->SetDestination(recv_msg->body.des_id());
				message->SetSource(recv_msg->body.src_id());
			}
			else if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_bytes))
			{
				JLOG(INFO) << "PoolConnection recv public_data_bytes, src_id: " << recv_msg->body.src_id();
				
				int len = recv_msg->RawDataLen();
				unique_ptr<unsigned char[]> buf(new unsigned char[len + 1]());
				memcpy(buf.get(), recv_msg->RawData(), len);
				buf[len] = 0;
				message = new JBytesMessageImpl(buf.get(), len);
				message->SetDestination(recv_msg->body.des_id());
				message->SetSource(recv_msg->body.src_id());
			}
			break;
		}
		
		index -= 100;
		usleep(1000*100);
	}
	return message;
}

