//
//  libjwumqa_destination.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/18.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_destination.hpp"
#include "libjwumqa_part_pipe.hpp"
#include "libjwumqa_part_member.hpp"
#include "libjwumqa_part_producer.hpp"
#include "libjwumqa_part_consumer.hpp"


JwumqDestinationImpl::JwumqDestinationImpl(string queue_name, uint64_t session):queue_name(queue_name), session(session)
{
	available = false;
	JLOG(INFO) << "JwumqDestinationImpl::JwumqDestinationImpl, q_name: " << this->queue_name.c_str() << ", session: " << this->session;
}

JwumqDestinationImpl::~JwumqDestinationImpl()
{
	JLOG(INFO) << "JwumqDestinationImpl::~JwumqDestinationImpl, q_name: " << this->queue_name.c_str() << ", session: " << this->session;
}

JwumqPartPipe*  JwumqDestinationImpl::CreatePartPipe(string node_id)
{
	JLOG(INFO) << "JwumqSessionImpl::CreatePartPipe, node id: " << node_id.c_str();
	
	JwumqPartPipeImpl* pipe = new JwumqPartPipeImpl(queue_url, queue_name, node_id, session);
	
	if(pipe->Setup() != JWUMQ_SUCCESS)
	{
		delete pipe;
		return nullptr;
	}
	if(!pipe->Available())
	{
		delete pipe;
		return nullptr;
	}
	return pipe;
}

JwumqPartMember*  JwumqDestinationImpl::CreatePartMember(string node_id)
{
	JLOG(INFO) << "JwumqSessionImpl::CreatePartMember, node id: " << node_id.c_str();
	
	JwumqPartMemberImpl* member = new JwumqPartMemberImpl(queue_url, queue_name, node_id, session);

	if(member->Setup() != JWUMQ_SUCCESS)
	{
		delete member;
		return nullptr;
	}
	if(!member->Available())
	{
		delete member;
		return nullptr;
	}
	return member;
}

JwumqPartProducer*  JwumqDestinationImpl::CreatePartProducer(string node_id)
{
	JLOG(INFO) << "JwumqSessionImpl::CreatePartProducer, node id: " << node_id.c_str();
	
	JwumqPartProducerImpl* producer = new JwumqPartProducerImpl(queue_url, queue_name, node_id, session);

	if(producer->Setup() != JWUMQ_SUCCESS)
	{
		delete producer;
		return nullptr;
	}
	if(!producer->Available())
	{
		delete producer;
		return nullptr;
	}
	return producer;
}

JwumqPartConsumer*  JwumqDestinationImpl::CreatePartConsumer(string node_id)
{
	JLOG(INFO) << "JwumqSessionImpl::CreatePartConsumer, node id: " << node_id.c_str();
	
	JwumqPartConsumerImpl* consumer = new JwumqPartConsumerImpl(queue_url, queue_name, node_id, session);

	if(consumer->Setup() != JWUMQ_SUCCESS)
	{
		delete consumer;
		return nullptr;
	}
	if(!consumer->Available())
	{
		delete consumer;
		return nullptr;
	}
	return consumer;
}

void JwumqDestinationImpl::CreateQueueRespListener(bool available, int queue_port)
{
	string factory_url = factory_conn->Url();
	string::size_type position = factory_url.find_last_of(":");
	if (position == std::string::npos)
	{
		return;
	}
	
	string url = factory_url.substr(0, position) + ":" + to_string(queue_port);

	this->available = available;
	JLOG(INFO) << "JwumqDestinationImpl recv private_create_queue_resp available: " << available << ", queue port: "
	<< queue_port << ", queue url: " << url.c_str();
	this->queue_url = url;
}

bool JwumqDestinationImpl::Available()
{
	return available;
}

void JwumqDestinationImpl::DestroyPart(JwumqPart* part)
{
	if(part != nullptr && dynamic_cast<JwumqPart*>(part))
	{
		delete part;
	}
}
