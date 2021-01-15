//
//  libjwumqa_session.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/18.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_session.hpp"
#include "libjwumqa_destination.hpp"
#include "JTextMessageImpl.hpp"
#include "JBytesMessageImpl.hpp"


JwumqSessionImpl::JwumqSessionImpl()
{
	JLOG(INFO) << "JwumqSessionImpl::JwumqSessionImpl";
}

JwumqSessionImpl::~JwumqSessionImpl()
{
	JLOG(INFO) << "JwumqSessionImpl::~JwumqSessionImpl";
}

JwumqDestination*  JwumqSessionImpl::CreateDestination(string queue_name)
{
	JLOG(INFO) << "JwumqSessionImpl::CreateDestination, q_name: " << queue_name.c_str() << ", session: " << this->session;
	this->queue_name = queue_name;
	
	JwumqDestinationImpl* des = new JwumqDestinationImpl(queue_name, session);
	des->factory_conn = factory_conn;
	
	auto key = factory_conn->create_queue_resp_event += std::bind(&JwumqDestinationImpl::CreateQueueRespListener, des, std::placeholders::_1, std::placeholders::_2);
	if(factory_conn->CreateQueue(session, queue_name, 5000) != JWUMQ_SUCCESS)
	{
		factory_conn->create_queue_resp_event -= key;
		delete des;
		return nullptr;
	}
	if(!des->Available())
	{
		delete des;
		return nullptr;
	}
	factory_conn->create_queue_resp_event -= key;
	
	return des;
	
}

void JwumqSessionImpl::SessionRespListener(uint64_t session)
{
	JLOG(INFO) << "JwumqSessionImpl recv private_session_resp " << session;
	this->session = session;
}

JTextMessage* JwumqSessionImpl::CreateTextMessage(string text)
{
	return new JTextMessageImpl(text);
}

JTextMessage* JwumqSessionImpl::CreateTextMessage()
{
	return new JTextMessageImpl();
}

JBytesMessage* JwumqSessionImpl::CreateBytesMessage(unsigned char * data, int length)
{
	return new JBytesMessageImpl(data, length);
}

JBytesMessage* JwumqSessionImpl::CreateBytesMessage()
{
	return new JBytesMessageImpl();
}

void JwumqSessionImpl::DestroyDestination(JwumqDestination* destination)
{
	if(destination != nullptr && dynamic_cast<JwumqDestination*>(destination))
	{
		delete destination;
	}
}

void JwumqSessionImpl::DestroyMessage(JMessage * message)
{
	if(message != nullptr)
	{
		delete message;
	}
}
