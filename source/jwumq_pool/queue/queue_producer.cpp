//
//  queue_producer.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "queue_producer.hpp"
#include "../../jlog/jLog.h"
#include "../../jwumq_protocol/jwumq_msg.pb.h"


QueueProducer::QueueProducer(string queue_name)
{
	this->queue_name = queue_name;
	part_count = 0;
}

QueueProducer::~QueueProducer()
{
	
}

int QueueProducer::AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type)
{
	if(type != JWUMQ_PART_TYPE_ENUM::pipe)
	{
		JLOG(INFO) << "Add part failed, because of the part type error: ";
		return jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_TYPE_ERROR;
	}
	std::unique_lock<std::mutex> lock(part_info_mutex);
	
	if(part_info.find(node_id) == part_info.end())
	{
		if(part_count >= MAX_PART_PIPE_COUNT)
		{
			JLOG(INFO) << "Add part failed, because of part is full, part name: " << PartName().c_str();
			return jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_IS_FULL;
		}
		
		PART_STATUS_T status = {node_id, 0, 0};
		part_info[node_id] = status;
		part_count ++;
		
		JLOG(INFO) << "Add part " << node_id.c_str() << " success, part count: " << part_count << ", part name: " << PartName().c_str();
		return jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS;
	}
	
	JLOG(INFO) << "Add part " << node_id.c_str() << " success, node has been exist, part name: " << PartName().c_str();
	return jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS;
}

int QueueProducer::SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage> recv_msg)
{
	string des = GetDesNode(recv_msg->body.src_id());
	if(des.length() > 0)
	{
		recv_msg->SetDestination(des);
		conn->Send(recv_msg);
	}
	
	return JWUMQ_SUCCESS;
}

int QueueProducer::SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage> recv_msg)
{
	string des = GetDesNode(recv_msg->body.src_id());
	if(des.length() > 0)
	{
		recv_msg->SetDestination(des);
		conn->Send(recv_msg);
	}
	
	return JWUMQ_SUCCESS;
}

string QueueProducer::GetDesNode(string src)
{
	string des = "";
	std::unique_lock<std::mutex> lock(part_info_mutex);
	for(auto &v : part_info)
	{
		if(v.first != src)
		{
			des = v.first;
			break;
		}
	}
	return des;
}


