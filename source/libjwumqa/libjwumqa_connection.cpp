//
//  libjwumqa_connection.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_connection.hpp"
#include "libjwumqa_session.hpp"
#include "../include/jwevents/jwevents.h"


JwumqConnectionImpl::JwumqConnectionImpl(string url):url(url)
{
	JLOG(INFO) << "JwumqConnectionImpl::JwumqConnectionImpl";
	factory_conn = make_shared<FactoryConnection>(url);
}

JwumqConnectionImpl::~JwumqConnectionImpl()
{
	JLOG(INFO) << "JwumqConnectionImpl::~JwumqConnectionImpl";
}

JwumqSession* JwumqConnectionImpl::CreateSession()
{
	JLOG(INFO) << "JwumqConnectionImpl::CreateSession";
	
	JwumqSessionImpl* session = new JwumqSessionImpl();
	auto key = factory_conn->session_resp_event += std::bind(&JwumqSessionImpl::SessionRespListener, session, std::placeholders::_1);
	if(factory_conn->RequestSession(5000) != JWUMQ_SUCCESS)
	{
		factory_conn->session_resp_event -= key;
		delete session;
		return nullptr;
	}
	factory_conn->session_resp_event -= key;
	session->factory_conn = factory_conn;
	return session;
}

int JwumqConnectionImpl::Connect()
{
	JLOG(INFO) << "JwumqConnectionImpl::Connect, url: " << url.c_str();
	return factory_conn->Connect();
}

void JwumqConnectionImpl::Close()
{
	JLOG(INFO) << "JwumqConnectionImpl::Close";
	factory_conn->Close();
}

void JwumqConnectionImpl::DestroySession(JwumqSession* session)
{
	if(session != nullptr && dynamic_cast<JwumqSession*>(session))
	{
		delete session;
	}
}

