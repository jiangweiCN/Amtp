//
//  proxy.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "proxy.hpp"
#include "../jlog/jLog.h"


Proxy::Proxy(string conf_file, string process_id)
{
	this->process_id = process_id;
	this->conf_p = make_unique<PROXY_CONF_T>();
	this->proxy_config_p = make_unique<ProxyConfig>(conf_file, process_id);
}

Proxy::~Proxy()
{
	proxy_config_p = nullptr;
	proxy.Release();
}

int Proxy::Setup()
{
	JWUMQ_RECV_CALLBACK callback = nullptr;
	proxy.Setup(conf_p.get(), callback);
	return 0;
}
bool Proxy::LoadConfig()
{
	bool res = proxy_config_p->Parse();
	if(!res)
	{
		proxy_config_p = nullptr;
		JLOG(ERROR) << "Config file load fail!";
		return false;
	}
	JLOG(INFO) <<  "Config file load success, version: " << proxy_config_p->version.c_str();
	JLOG(INFO) <<  proxy_config_p->Print().c_str();
	
	memset(conf_p.get(), 0, sizeof(PROXY_CONF_T));
	
	memcpy(conf_p->identity, process_id.c_str(), process_id.length());
	memcpy(conf_p->frontend_addr, proxy_config_p->config_p->frontend_address.c_str(), proxy_config_p->config_p->frontend_address.length());
	memcpy(conf_p->backend_addr, proxy_config_p->config_p->backend_address.c_str(), proxy_config_p->config_p->backend_address.length());

	if(proxy_config_p->config_p->frontend_type.compare ("router") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::router;
	}
	else if(proxy_config_p->config_p->frontend_type.compare ("dealer") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::dealer;
	}
	else if(proxy_config_p->config_p->frontend_type.compare ("xpub") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::xpub;
	}
	else if(proxy_config_p->config_p->frontend_type.compare ("xsub") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::xsub;
	}
	else
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::unknown;
		return false;
	}
	
	if(proxy_config_p->config_p->backend_type.compare ("router") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::router;
	}
	else if(proxy_config_p->config_p->backend_type.compare ("dealer") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::dealer;
	}
	else if(proxy_config_p->config_p->backend_type.compare ("xpub") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::xpub;
	}
	else if(proxy_config_p->config_p->backend_type.compare ("xsub") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::xsub;
	}
	else
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::unknown;
		return false;
	}
	
	conf_p->type = JWUMQ_TYPE_ENUM::proxy;
	conf_p->frontend_bind = proxy_config_p->config_p->frontend_bind;
	conf_p->backend_bind = proxy_config_p->config_p->backend_bind;
	
	return true;
}
void Proxy::Release()
{
	proxy.Release();
}
