//
//  libjwumqa_part_consumer.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_part_consumer.hpp"
#include "../jwumq_protocol/jwumq_msg.pb.h"


JwumqPartConsumerImpl::JwumqPartConsumerImpl(string url, string queue_name, string node_id, uint64_t session):url(url), queue_name(queue_name), node_id(node_id), session(session)
{
	JLOG(INFO) << "JwumqPartConsumerImpl::JwumqPartConsumerImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
	available = false;
	conn = make_unique<PoolConnection>(url, queue_name, node_id);
	conn->create_part_consumer_resp_event += std::bind(&JwumqPartConsumerImpl::PartConsumerRespListener, this, std::placeholders::_1);
}

JwumqPartConsumerImpl::~JwumqPartConsumerImpl()
{
	Release();
	JLOG(INFO) << "JwumqPartConsumerImpl::~JwumqPartConsumerImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
}

int JwumqPartConsumerImpl::Setup()
{
	int result = conn->Connect();
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	result = conn->CreatePartConsumer(5000);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	return 0;
}

bool JwumqPartConsumerImpl::Available()
{
	return available;
}

void JwumqPartConsumerImpl::PartConsumerRespListener(uint32_t result)
{
	if(result == jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS)
	{
		available = true;
	}
}

int JwumqPartConsumerImpl::Send(JMessage * message)
{
	return conn->Send(message);
}

JMessage * JwumqPartConsumerImpl::Receive(int timeout)
{
	return conn->Receive(timeout);
}

void JwumqPartConsumerImpl::Release()
{
	available = false;
	if(conn != nullptr)
	{
		conn->Close();
	}
}

