//
//  queue_member.cpp
//  Amtp
//
//  Created by jiangwei on 2020/7/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "queue_member.hpp"
#include <iterator>
#include <regex>
#include "../../jlog/jLog.h"
#include "../../jwumq_protocol/jwumq_msg.pb.h"


QueueMember::QueueMember(string queue_name)
{
	this->queue_name = queue_name;
	part_count = 0;
}

QueueMember::~QueueMember()
{
	
}

int QueueMember::AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type)
{
	if(type != JWUMQ_PART_TYPE_ENUM::member)
	{
		JLOG(INFO) << "Add part failed, because of the part type error: ";
		return jwumq_pb_msg::JWUMQ_MSG_RESULT_E::QUEUE_TYPE_ERROR;
	}
	std::unique_lock<std::mutex> lock(part_info_mutex);
	
	if(part_info.find(node_id) == part_info.end())
	{
		if(part_count >= MAX_PART_MEMBER_COUNT)
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

int QueueMember::SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage> recv_msg)
{
	vector<string> des_list = GetDesNode(recv_msg->body.src_id(), recv_msg->body.des_id());
	for(auto des:des_list)
	{
		shared_ptr<JwumqMessage> send_msg(new JwumqMessage(recv_msg.get()));
		send_msg->SetDestination(des);
		conn->Send(send_msg);
	}
	
	return JWUMQ_SUCCESS;
}

int QueueMember::SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage> recv_msg)
{
	vector<string> des_list = GetDesNode(recv_msg->body.src_id(), recv_msg->body.des_id());
	for(auto des:des_list)
	{
		shared_ptr<JwumqMessage> send_msg(new JwumqMessage(recv_msg.get()));
		send_msg->SetDestination(des);
		conn->Send(send_msg);
	}
	return JWUMQ_SUCCESS;
}

vector<string> QueueMember::GetDesNode(string src, string des)
{
	vector<string> des_list;
	if(des.length() > 0)
	{
		string delim = ",";
		std::regex re{"[,]+"};
		des_list = std::vector<std::string> {
			std::sregex_token_iterator(des.begin(), des.end(), re, -1),
			std::sregex_token_iterator()
		};
	}
	else if(des.length() == 0)
	{
		std::unique_lock<std::mutex> lock(part_info_mutex);
		for(auto &v : part_info)
		{
			if(v.first != src)
			{
				des_list.push_back(v.first);
			}
		}
	}
	
	return des_list;
}

