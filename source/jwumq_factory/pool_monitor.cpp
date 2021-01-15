//
//  pool_monitor.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "pool_monitor.hpp"
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"
#include "factory_def.h"


PoolMonitor::PoolMonitor(string id)
{
	jwumq_dp = make_unique<JwumqDuplexPub>();
	this->factory_id = id;
	sn = 0;
}

PoolMonitor::~PoolMonitor()
{
	loop = false;
}

bool PoolMonitor::Start()
{
	JLOG(INFO) << "Pool monitor start.";

	DUPLEX_PUB_SUB_CONF_T conf_t;
	conf_t.type = JWUMQ_TYPE_ENUM::duplex_pub;
	memcpy(conf_t.identity, factory_id.c_str(), factory_id.length());
	conf_t.identity[factory_id.length()] = 0;
	
	string pub_address = "ipc:///tmp/jwumq_factory_status_pub";
	memcpy(conf_t.pub_addr, pub_address.c_str(), pub_address.length());
	conf_t.pub_addr[pub_address.length()] = 0;
	
	string dealer_addr = "ipc:///tmp/jwumq_factory_status_dealer";
	memcpy(conf_t.dealer_addr, dealer_addr.c_str(), dealer_addr.length());
	conf_t.dealer_addr[dealer_addr.length()] = 0;
	
	int result = jwumq_dp->Setup(&conf_t, std::bind(&PoolMonitor::RecvCallback, this
													, std::placeholders::_1));
	if(result != LIB_JWUMQ_SUCCESS)
	{
		JLOG(ERROR) <<  "Failed to setup duplex_pub, error: " << result;
		return false;
	}
	
	loop = true;
	send_thread = thread(&PoolMonitor::SendThread, this);
	recv_thread = thread(&PoolMonitor::RecvThread, this);
	status_query_thread = thread(&PoolMonitor::StatusQueryThread, this);

	return true;
}

bool PoolMonitor::End()
{
	loop = false;
	jwumq_dp->Release();
	if(status_query_thread.joinable())
	{
		status_query_thread.join();
	}
	if(send_thread.joinable())
	{
		send_thread.join();
	}
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	queue<void *> empty_s;
	swap(empty_s, send_q);
	
	queue<shared_ptr<JwumqMessage>> empty_r;
	swap(empty_r, recv_q);
	return true;
}

int PoolMonitor::RecvCallback(void * recv_msg)
{
	shared_ptr<JwumqMessage> msg(new JwumqMessage((JwumqMessage *)recv_msg));
//	JLOG(INFO) << "Recv pool msg! " << msg->MsgDataLen();
	std::unique_lock<std::mutex> lock(recv_mutex);
	recv_q.push(msg);
	return 0;
}

void PoolMonitor::StatusQueryThread()
{
	JLOG(INFO) << "Status query thread begin! ";

	time_t l_t = 0, c_t = 0;
	sleep(1);
	while (loop)
	{
		c_t = time(NULL);
		if(c_t - l_t >= QUERY_STATUS_INTERVAL_SEC)
		{
			l_t = c_t;
			jwumq_pb_msg::PoolStatusRequest req;
			req.set_sn(++sn);

			size_t req_len = req.ByteSizeLong();
			unique_ptr<char[]> req_buf(new char[req_len]());
			req.SerializeToArray(req_buf.get(),req_len);

			JwumqMessage * msg = new JwumqMessage(JWUMQ_COMMAND_ENUM::private_pool_status_request, factory_id, "all", req_buf.get(), (int)req_len);
			std::unique_lock<std::mutex> lock(send_mutex);
			send_q.push(msg);
		}
		usleep(1000*100);
	}
	JLOG(INFO) << "Status query thread end! ";
}

void PoolMonitor::SendThread()
{
	JLOG(INFO) << "Pool monitor send thread begin! ";
	while (loop)
	{
		if(send_q.size() <= 0)
		{
			usleep(1000 * 10);
			continue;
		}
		
		JwumqMessage * msg = nullptr;
		
		std::unique_lock<std::mutex> lock(send_mutex, std::defer_lock);
		lock.lock();
		msg = (JwumqMessage *)send_q.front();
		send_q.pop();
		lock.unlock();
		
		if(msg != nullptr)
		{
			int result = jwumq_dp->Send(msg);
			JLOG(INFO) << "Send dp msg, cmd: " << static_cast<uint32_t>(msg->body.command())
						<< ", result: " << result;
			delete msg;
		}
	}
	JLOG(INFO) << "Pool monitor send thread end! ";
}

void PoolMonitor::RecvThread()
{
	JLOG(INFO) << "Pool monitor recv thread begin! ";
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
		shared_ptr<JwumqMessage> msg = recv_q.front();
		recv_q.pop();
		ptx->unlock();
		
		switch(msg->body.command())
		{
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_pool_status_report):
			{
				JLOG(INFO) << "Recv " << msg->body.src_id().c_str()
					<< " status report.";
				pool_status_report_event(msg);
				break;
			}
			default:break;
		}
	}
	JLOG(INFO) << "Pool monitor recv thread end! ";
}

void PoolMonitor::AssignedQueue(string pool_name, uint64_t session, string queue_name)
{
	jwumq_pb_msg::AssignedQueueRequest req;
	req.set_sn(++sn);
	req.set_session(session);
	req.set_queue_name(queue_name);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(),req_len);
	
	string pool_id = pool_name + "_sub";

	JwumqMessage * msg = new JwumqMessage(JWUMQ_COMMAND_ENUM::private_Assigned_queue_req, factory_id, pool_id, req_buf.get(), (int)req_len);
	std::unique_lock<std::mutex> lock(send_mutex);
	send_q.push(msg);
}


