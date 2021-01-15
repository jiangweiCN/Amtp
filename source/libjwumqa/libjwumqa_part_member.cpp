//
//  libjwumqa_part_member.cpp
//  Amtp
//
//  Created by jiangwei on 2020/7/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_part_member.hpp"
#include "../jwumq_protocol/jwumq_msg.pb.h"


JwumqPartMemberImpl::JwumqPartMemberImpl(string url, string queue_name, string node_id, uint64_t session):url(url), queue_name(queue_name), node_id(node_id), session(session)
{
	JLOG(INFO) << "JwumqPartMemberImpl::JwumqPartMemberImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
	available = false;
	conn = make_unique<PoolConnection>(url, queue_name, node_id);
	conn->create_part_member_resp_event += std::bind(&JwumqPartMemberImpl::PartMemberRespListener, this, std::placeholders::_1);
}

JwumqPartMemberImpl::~JwumqPartMemberImpl()
{
	Release();
	JLOG(INFO) << "JwumqPartMemberImpl::~JwumqPartMemberImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
}

int JwumqPartMemberImpl::Setup()
{
	int result = conn->Connect();
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	result = conn->CreatePartMember(5000);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	return 0;
}

bool JwumqPartMemberImpl::Available()
{
	return available;
}

void JwumqPartMemberImpl::PartMemberRespListener(uint32_t result)
{
	if(result == jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS)
	{
		available = true;
	}
}

int JwumqPartMemberImpl::Send(JMessage * message)
{
	return conn->Send(message);
}

JMessage * JwumqPartMemberImpl::Receive(int timeout)
{
	return conn->Receive(timeout);
}

void JwumqPartMemberImpl::Release()
{
	available = false;
	if(conn != nullptr)
	{
		conn->Close();
	}
}

