//
//  libjwumqa_factory.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/17.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumqa_cpp.h"
#include "libjwumqa_connection.hpp"
#include "../jlog/jLog.h"




JwumqFactory::JwumqFactory()
{
	
}

JwumqFactory::JwumqFactory(string url):factory_url(url)
{
	JLOG(INFO) << "JwumqFactory::JwumqFactory, factory_url: " << factory_url.c_str();
}

JwumqFactory::~JwumqFactory()
{
	JLOG(INFO) << "JwumqFactory::~JwumqFactory, factory_url: " << factory_url.c_str();
}

void JwumqFactory::SetUrl(string url)
{
	factory_url = url;
}

JwumqConnection* JwumqFactory::CreateConnection()
{
	JLOG(INFO) << "JwumqFactory::CreateConnection, factory_url: " << factory_url.c_str();

	return new JwumqConnectionImpl(factory_url);
}

void JwumqFactory::DestroyConnection(JwumqConnection* conn)
{
	if(conn != nullptr && dynamic_cast<JwumqConnection*>(conn))
	{
		delete conn;
	}
}
