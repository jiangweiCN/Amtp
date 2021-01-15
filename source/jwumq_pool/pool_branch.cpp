//
//  pool_branch.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "pool_branch.hpp"
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"


PoolBranch::PoolBranch(string id, uint32_t port, shared_ptr<PoolStatus> status)
{
	jwumq_ds = make_unique<JwumqDuplexSub>();
	this->pool_id = id;
	this->status = status;
	this->access_port = port;
}

PoolBranch::~PoolBranch()
{
	loop = false;
}

bool PoolBranch::Start()
{
	JLOG(INFO) << "Pool branch start.";

	DUPLEX_PUB_SUB_CONF_T conf_t;
	conf_t.type = JWUMQ_TYPE_ENUM::duplex_sub;
	memcpy(conf_t.identity, pool_id.c_str(), pool_id.length());
	conf_t.identity[pool_id.length()] = 0;
	
	string pub_address = "ipc:///tmp/jwumq_factory_status_pub";
	memcpy(conf_t.pub_addr, pub_address.c_str(), pub_address.length());
	conf_t.pub_addr[pub_address.length()] = 0;
	
	string dealer_addr = "ipc:///tmp/jwumq_factory_status_dealer";
	memcpy(conf_t.dealer_addr, dealer_addr.c_str(), dealer_addr.length());
	conf_t.dealer_addr[dealer_addr.length()] = 0;
	
	int result = jwumq_ds->Setup(&conf_t, std::bind(&PoolBranch::RecvCallback, this
													, std::placeholders::_1));
	if(result != LIB_JWUMQ_SUCCESS)
	{
		JLOG(ERROR) <<  "Failed to setup duplex_sub, error: " << result;
		return false;
	}
	
	loop = true;
	send_thread = thread(&PoolBranch::SendThread, this);

	return true;
}

bool PoolBranch::End()
{
	loop = false;
	jwumq_ds->Release();
	
	if(send_thread.joinable())
	{
		send_thread.join();
	}
	queue<void *> empty;
	swap(empty, send_q);
	return true;
}

int PoolBranch::RecvCallback(void * msg)
{
	if(msg == nullptr)
	{
		return -1;
	}
	JwumqMessage * recv_msg = (JwumqMessage*) msg;
	switch(recv_msg->body.command())
	{
		case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_pool_status_request):
		{
			int msg_len = recv_msg->RawDataLen();
			jwumq_pb_msg::PoolStatusRequest req;
			req.ParseFromArray(recv_msg->RawData(), msg_len);
			int sn = req.sn();
			JLOG(INFO) << "Recv factory pool_status_request, sn: " << sn;
			status->UpdatePoolPrivate(recv_msg->MsgDataLen(), 1);
			SendStatus(sn, recv_msg->body.src_id().c_str());
			break;
		}
		case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_Assigned_queue_req):
		{
			int msg_len = recv_msg->RawDataLen();
			jwumq_pb_msg::AssignedQueueRequest req;
			req.ParseFromArray(recv_msg->RawData(), msg_len);
			int sn = req.sn();
			uint64_t session = req.session();
			string queue_name = req.queue_name();
			JLOG(INFO) << "Recv factory private_Assigned_queue_req, sn: " << sn
			<< ", session: " << session << ", queue_name: " << queue_name.c_str();
			assigned_queue_event(queue_name);
			status->UpdatePoolPrivate(recv_msg->MsgDataLen(), 1);
			
			break;
		}
		default:break;
	}
	return 0;
}

void PoolBranch::SendThread()
{
	JLOG(INFO) << "Pool branch send thread begin! ";
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
			int result = jwumq_ds->Send(msg);
			JLOG(INFO) << "Send ds msg, cmd: " << static_cast<uint32_t>(msg->body.command())
						<< ", msg length: " << msg->MsgDataLen()
						<< ", result: " << result;
			delete msg;
		}
	}
	JLOG(INFO) << "Pool branch send thread end! ";
}

void PoolBranch::SendStatus(int request_sn, string des_id)
{
	time_t c_t = time(NULL);
	jwumq_pb_msg::PoolStatusReport report;
	
	status->Reset(report);
	report.set_name(pool_id);
	report.set_sn(request_sn);
	report.set_access_port(access_port);
	report.set_report_time(c_t);
	
	size_t data_len = report.ByteSizeLong();
	unique_ptr<char[]> data_buf(new char[data_len]());
	report.SerializeToArray(data_buf.get(), (int)data_len);
	JwumqMessage * msg = new JwumqMessage(JWUMQ_COMMAND_ENUM::private_pool_status_report, pool_id, des_id, data_buf.get(), (int)data_len);
	std::unique_lock<std::mutex> lock(send_mutex);
	send_q.push(msg);
}



