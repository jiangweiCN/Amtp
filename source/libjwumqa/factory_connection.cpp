//
//  factory_connection.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "factory_connection.hpp"
#include <sys/time.h>
#include <unistd.h>
#include "../jwumq_protocol/jwumq_msg.pb.h"


FactoryConnection::FactoryConnection(string url):url(url)
{
	JLOG(INFO) << "FactoryConnection::FactoryConnection";
	conn = make_unique<JwumqDealer>();
}

FactoryConnection::~FactoryConnection()
{
	JLOG(INFO) << "FactoryConnection::~FactoryConnection";
}

int FactoryConnection::Connect()
{
	JLOG(INFO) << "FactoryConnection::Connect, url: " << url.c_str();
	
	int result = Setup();
	JLOG(INFO) << "FactoryConnection::Connect, result: " << result;
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}

	loop = true;
	send_thread = thread(&FactoryConnection::SendThread, this);
	recv_thread = thread(&FactoryConnection::RecvThread, this);
	alive_thread = thread(&FactoryConnection::AliveThread, this);
	
	return JWUMQ_SUCCESS;
}

void FactoryConnection::Close()
{
	JLOG(INFO) << "FactoryConnection::Close ";
	loop = false;
	if(send_thread.joinable())
	{
		send_thread.join();
	}
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	if(alive_thread.joinable())
	{
		alive_thread.join();
	}
	conn->Release();
}

int FactoryConnection::RecvCallback(void * recv_msg)
{
//	fprintf(stderr, "JwumqConnectionImpl::RecvCallback\n");
	shared_ptr<JwumqMessage> msg(new JwumqMessage((JwumqMessage *)recv_msg));
	std::unique_lock<std::mutex> lock(recv_mutex);
	recv_q.push(msg);
	
	return 0;
}

int FactoryConnection::Setup()
{
	struct timeval tv;
    gettimeofday(&tv,NULL);
	identity = to_string(tv.tv_sec*1000000 + tv.tv_usec);
	JLOG(INFO) << "FactoryConnection identity: " << identity.c_str();


	DEALER_CONF_T conf_t;
	memcpy(conf_t.address, url.c_str(), url.length());
	conf_t.address[url.length()] = 0;
	memcpy(conf_t.identity, identity.c_str(), identity.length());
	conf_t.identity[identity.length()] = 0;
	conf_t.linger = 1;
	conf_t.read_timeout = 5000;
	conf_t.bind = false;

	return conn->Setup(&conf_t, std::bind(&FactoryConnection::RecvCallback, this
	, std::placeholders::_1));
}

void FactoryConnection::AliveThread()
{
	time_t l_t = 0, c_t = 0;
	while (loop)
	{
		c_t = time(NULL);
		if(c_t - l_t >= 10)
		{
			l_t = c_t;
			RequestAlive(3000);
		}
		usleep(1000*10);
	}
}

int FactoryConnection::RequestAlive(int timeout)
{
	string alive = identity + ", alive";
	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, identity, "",  alive.c_str(), (int)alive.length()));
	std::unique_lock<std::mutex> lock(send_mutex);
	send_q.push(msg);
	JLOG(INFO) << "FactoryConnection push alive!";
	return JWUMQ_SUCCESS;
}

int FactoryConnection::RequestSession(int timeout)
{
	string alive = identity + ", session";
	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_session_req, identity, "",  alive.c_str(), (int)alive.length()));
	std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "FactoryConnection push session request!";
	
	std::unique_lock<std::mutex> ack_lock(req_session_mutex);
	if(req_session_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	
	return JWUMQ_SUCCESS;
}

int FactoryConnection::CreateQueue(uint64_t session, string queue_name, int timeout)
{
	jwumq_pb_msg::CreateQueueRequest req;
	req.set_session(session);
	req.set_queue_name(queue_name);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);
	
	shared_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_create_queue_req, identity, "",  req_buf.get(), (int)req_len));
	std::unique_lock<std::mutex> lock(req_create_queue_mutex, std::defer_lock);
	lock.lock();
	send_q.push(msg);
	lock.unlock();
	JLOG(INFO) << "FactoryConnection push create queue request!";
	
	std::unique_lock<std::mutex> ack_lock(req_create_queue_mutex);
	if(req_create_queue_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return JWUMQ_SEND_TIMEOUT;
	}
	return JWUMQ_SUCCESS;
}

void FactoryConnection::SendThread()
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

void FactoryConnection::RecvThread()
{
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
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_resp):
			{
				int data_len = recv_msg->RawDataLen();
				unique_ptr<char[]> buf(new char[data_len + 1]());
				memcpy(buf.get(), recv_msg->RawData(), data_len);
				buf[data_len] = 0;
				JLOG(INFO) << "Recv private_alive_resp " << buf.get();
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_session_resp):
			{
				jwumq_pb_msg::SessionResponse resp;
				resp.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				
				uint64_t session = resp.session();
				
				JLOG(INFO) << "FactoryConnection recv private_session_resp " << session;
				session_resp_event(session);
				req_session_cv.notify_all();
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_create_queue_resp):
			{
				jwumq_pb_msg::CreateQueueResponse resp;
				resp.ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());
				
				uint64_t session = resp.session();
				string queue_name = resp.queue_name();
				int queue_port = resp.queue_port();
				uint32_t result = resp.result();
				
				JLOG(INFO) << "FactoryConnection recv private_create_queue_resp, session: " << session << ", queue_name: "
				<< queue_name.c_str() << ", queue_port: " << queue_port << ", result: " << result;
				bool available = false;
				if(result == jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS)
				{
					available = true;
				}
				create_queue_resp_event(available, queue_port);
				req_create_queue_cv.notify_all();
				break;
			}
			default:break;
		}
	}
}
string FactoryConnection::Url()
{
	return url;
}

