//
//  router.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "router.hpp"
#include "../jlog/jLog.h"


Router::Router(string conf_file, string process_id)
{
	this->process_id = process_id;
	this->conf_p = make_unique<ROUTER_CONF_T>();
	this->router_config_p = make_unique<RouterConfig>(conf_file, process_id);
}

Router::~Router()
{
	router_config_p = nullptr;
	router.Release();
}

int Router::Setup()
{
	JWUMQ_RECV_CALLBACK callback = nullptr;
	router.Setup(conf_p.get(), callback);
	return 0;
}
bool Router::LoadConfig()
{
//	router_config_p = nullptr;
//	router_config_p = make_unique<RouterConfig>(process_id);
	bool res = router_config_p->Parse();
	if(!res)
	{
		router_config_p = nullptr;
		JLOG(ERROR) << "Config file load fail!";
		return false;
	}
	JLOG(INFO) <<  "Config file load success, version: " << router_config_p->version.c_str();
	JLOG(INFO) <<  router_config_p->Print().c_str();
	
	memset(conf_p.get(), 0, sizeof(ROUTER_CONF_T));
	memcpy(conf_p->identity, process_id.c_str(), process_id.length());
	memcpy(conf_p->frontend_addr, router_config_p->config_p->frontend_address.c_str(), router_config_p->config_p->frontend_address.length());
	memcpy(conf_p->backend_addr, router_config_p->config_p->backend_address.c_str(), router_config_p->config_p->backend_address.length());

	if(router_config_p->config_p->frontend_type.compare ("router") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::router;
	}
	else if(router_config_p->config_p->frontend_type.compare ("dealer") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::dealer;
	}
	else if(router_config_p->config_p->frontend_type.compare ("xpub") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::xpub;
	}
	else if(router_config_p->config_p->frontend_type.compare ("xsub") == 0)
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::xsub;
	}
	else
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::unknown;
		return false;
	}
	
	if(router_config_p->config_p->backend_type.compare ("router") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::router;
	}
	else if(router_config_p->config_p->backend_type.compare ("dealer") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::dealer;
	}
	else if(router_config_p->config_p->backend_type.compare ("xpub") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::xpub;
	}
	else if(router_config_p->config_p->backend_type.compare ("xsub") == 0)
	{
		conf_p->backend_type = ZMQ_TYPE_ENUM::xsub;
	}
	else
	{
		conf_p->frontend_type = ZMQ_TYPE_ENUM::unknown;
		return false;
	}
	
	conf_p->type = JWUMQ_TYPE_ENUM::router;
	conf_p->frontend_bind = router_config_p->config_p->frontend_bind;
	conf_p->backend_bind = router_config_p->config_p->backend_bind;
	
	return true;
}
void Router::Release()
{
	router.Release();
}




