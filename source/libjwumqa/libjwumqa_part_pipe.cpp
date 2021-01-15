//
//  libjwumqa_part_pipe.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_part_pipe.hpp"
#include "../jwumq_protocol/jwumq_msg.pb.h"


JwumqPartPipeImpl::JwumqPartPipeImpl(string url, string queue_name, string node_id, uint64_t session):url(url), queue_name(queue_name), node_id(node_id), session(session)
{
	JLOG(INFO) << "JwumqPartPipeImpl::JwumqPartPipeImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
	available = false;
	conn = make_unique<PoolConnection>(url, queue_name, node_id);
	conn->create_part_pipe_resp_event += std::bind(&JwumqPartPipeImpl::PartPipeRespListener, this, std::placeholders::_1);
}

JwumqPartPipeImpl::~JwumqPartPipeImpl()
{
	Release();
	JLOG(INFO) << "JwumqPartPipeImpl::~JwumqPartPipeImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
}

int JwumqPartPipeImpl::Setup()
{
	int result = conn->Connect();
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	result = conn->CreatePartPipe(5000);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	return 0;
}

bool JwumqPartPipeImpl::Available()
{
	return available;
}

void JwumqPartPipeImpl::PartPipeRespListener(uint32_t result)
{
	if(result == jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS)
	{
		available = true;
	}
}

int JwumqPartPipeImpl::Send(JMessage * message)
{
	return conn->Send(message);
}

JMessage * JwumqPartPipeImpl::Receive(int timeout)
{
	return conn->Receive(timeout);
}

void JwumqPartPipeImpl::Release()
{
	available = false;
	if(conn != nullptr)
	{
		conn->Close();
	}
}
