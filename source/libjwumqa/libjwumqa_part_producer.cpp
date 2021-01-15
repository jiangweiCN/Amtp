//
//  libjwumqa_part_producer.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_part_producer.hpp"
#include "../jwumq_protocol/jwumq_msg.pb.h"


JwumqPartProducerImpl::JwumqPartProducerImpl(string url, string queue_name, string node_id, uint64_t session):url(url), queue_name(queue_name), node_id(node_id), session(session)
{
	JLOG(INFO) << "JwumqPartProducerImpl::JwumqPartProducerImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
	available = false;
	conn = make_unique<PoolConnection>(url, queue_name, node_id);
	conn->create_part_producer_resp_event += std::bind(&JwumqPartProducerImpl::PartProducerRespListener, this, std::placeholders::_1);
}

JwumqPartProducerImpl::~JwumqPartProducerImpl()
{
	Release();
	JLOG(INFO) << "JwumqPartProducerImpl::~JwumqPartProducerImpl, q_name: " << this->queue_name.c_str() << ", node id: "
	<< this->node_id.c_str() << ", session: " << this->session;
}

int JwumqPartProducerImpl::Setup()
{
	int result = conn->Connect();
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	result = conn->CreatePartProducer(5000);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		return result;
	}
	return 0;
}

bool JwumqPartProducerImpl::Available()
{
	return available;
}

void JwumqPartProducerImpl::PartProducerRespListener(uint32_t result)
{
	if(result == jwumq_pb_msg::JWUMQ_MSG_RESULT_E::SUCCESS)
	{
		available = true;
	}
}

int JwumqPartProducerImpl::Send(JMessage * message)
{
	return conn->Send(message);
}

JMessage * JwumqPartProducerImpl::Receive(int timeout)
{
	return conn->Receive(timeout);
}

void JwumqPartProducerImpl::Release()
{
	available = false;
	if(conn != nullptr)
	{
		conn->Close();
	}
}

