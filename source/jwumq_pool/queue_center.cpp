//
//  queue_center.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "queue_center.hpp"
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"
#include "queue/queue_pipe.hpp"
#include "queue/queue_member.hpp"
#include "queue/queue_producer.hpp"


QueueCenter::QueueCenter(string id, uint32_t port, shared_ptr<PoolStatus> status):pool_id(id),access_port(port), status(status)
{
	access_unit = make_shared<AccessUnit>(id, port, status);
	access_unit->create_part_pipe_event += std::bind(&QueueCenter::CreatePartPipeListener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	access_unit->create_part_member_event += std::bind(&QueueCenter::CreatePartMemberListener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	access_unit->create_part_producer_event += std::bind(&QueueCenter::CreatePartProducerListener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	access_unit->create_part_consumer_event += std::bind(&QueueCenter::CreatePartConsumerListener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	access_unit->recv_public_text_event += std::bind(&QueueCenter::RecvPublicTextListener, this, std::placeholders::_1);
	access_unit->recv_public_bytes_event += std::bind(&QueueCenter::RecvPublicTextListener, this, std::placeholders::_1);
}

QueueCenter::~QueueCenter()
{
	
}

bool QueueCenter::Start()
{
	JLOG(INFO) << "Queue center start.";

	queue_info_map.clear();
	if(!access_unit->Start())
	{
		JLOG(ERROR) << "Access unit start failed";
	}

	return true;
}

bool QueueCenter::End()
{
	access_unit->End();

	return true;
}

void QueueCenter::AssignedQueueListener(string queue_name)
{
	JLOG(INFO) << "Queue center assigned queue, queue name: " << queue_name.c_str();
	if(queue_info_map.find(queue_name) == queue_info_map.end())
	{
		queue_info_map[queue_name] = nullptr;
	}
}

bool QueueCenter::FindQueue(string queue_name)
{
	bool result = true;
	if(queue_info_map.find(queue_name) == queue_info_map.end())
	{
		JLOG(INFO) << "Queue center the queue is not exist, queue name: " << queue_name.c_str();
		result = false;
	}
	return result;
}
void QueueCenter::CreatePartPipeListener(string queue_name, string node_id, int &result)
{
	if(!FindQueue(queue_name))
	{
		JLOG(INFO) << "Queue center create part pipe queue failed, queue name: " << queue_name.c_str();
		result = jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_NOT_EXIST;
		return;
	}
	shared_ptr<QueueBase> queue_pipe = queue_info_map[queue_name];
	if(queue_pipe == nullptr)
	{
		JLOG(INFO) << "Queue center initialize part pipe queue, queue name: " << queue_name.c_str();
		queue_pipe = make_shared<QueuePipe>(queue_name);
		queue_info_map[queue_name] = queue_pipe;
	}
	result = queue_pipe->AddPart(node_id, JWUMQ_PART_TYPE_ENUM::pipe);
}

void QueueCenter::CreatePartMemberListener(string queue_name, string node_id, int &result)
{
	if(!FindQueue(queue_name))
	{
		JLOG(INFO) << "Queue center create part member queue failed, queue name: " << queue_name.c_str();
		result = jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_NOT_EXIST;
		return;
	}
	shared_ptr<QueueBase> queue_member = queue_info_map[queue_name];
	if(queue_member == nullptr)
	{
		JLOG(INFO) << "Queue center initialize part member queue, queue name: " << queue_name.c_str();
		queue_member = make_shared<QueueMember>(queue_name);
		queue_info_map[queue_name] = queue_member;
	}
	result = queue_member->AddPart(node_id, JWUMQ_PART_TYPE_ENUM::member);
}

void QueueCenter::CreatePartProducerListener(string queue_name, string node_id, int &result)
{
	if(!FindQueue(queue_name))
	{
		JLOG(INFO) << "Queue center create part producer queue failed, queue name: " << queue_name.c_str();
		result = jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_NOT_EXIST;
		return;
	}
	shared_ptr<QueueBase> queue_producer = queue_info_map[queue_name];
	if(queue_producer == nullptr)
	{
		JLOG(INFO) << "Queue center initialize part producer queue, queue name: " << queue_name.c_str();
		queue_producer = make_shared<QueueProducer>(queue_name);
		queue_info_map[queue_name] = queue_producer;
	}
	result = queue_producer->AddPart(node_id, JWUMQ_PART_TYPE_ENUM::pipe);
}
void QueueCenter::CreatePartConsumerListener(string queue_name, string node_id, int &result)
{
	if(!FindQueue(queue_name))
	{
		JLOG(INFO) << "Queue center create part consumer queue failed, queue name: " << queue_name.c_str();
		result = jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_NOT_EXIST;
		return;
	}
	shared_ptr<QueueBase> queue_consumer = queue_info_map[queue_name];
	if(queue_consumer == nullptr)
	{
		JLOG(INFO) << "Queue center initialize part consumer queue, queue name: " << queue_name.c_str();
		queue_consumer = make_shared<QueueProducer>(queue_name);
		queue_info_map[queue_name] = queue_consumer;
	}
	result = queue_consumer->AddPart(node_id, JWUMQ_PART_TYPE_ENUM::pipe);
}

void QueueCenter::RecvPublicTextListener(shared_ptr<JwumqMessage> recv_msg)
{
	std::unique_lock<std::mutex> queue_status_lock(queue_info_mutex);
	string queue_name = recv_msg->body.queue_name();
	if(queue_info_map.find(queue_name) != queue_info_map.end())
	{
		shared_ptr<QueueBase> _queue = queue_info_map[queue_name];
		_queue->SendText(access_unit, recv_msg);
		return;
	}
}

void QueueCenter::RecvPublicBytesListener(shared_ptr<JwumqMessage> recv_msg)
{
	std::unique_lock<std::mutex> queue_status_lock(queue_info_mutex);
	string queue_name = recv_msg->body.queue_name();
	if(queue_info_map.find(queue_name) != queue_info_map.end())
	{
		shared_ptr<QueueBase> _queue = queue_info_map[queue_name];
		_queue->SendBytes(access_unit, recv_msg);
		return;
	}
}
