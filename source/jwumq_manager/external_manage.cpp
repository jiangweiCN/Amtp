//
//  external_manage.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/14.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "external_manage.hpp"
#include <dlfcn.h>
#include <memory>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../jlog/jLog.h"
#include "../jwumq/jwumq_message.hpp"
#include "../include/cjson/cJSON.h"
#include "../config/monitor_config.h"


ExternalManage::ExternalManage()
{
	lib_handle = NULL;
	lib_version = NULL;
	lib_jwumq_setup = NULL;
	lib_jwumq_send = NULL;
	lib_jwumq_release = NULL;
	jmq_handle = NULL;
	self_id = "jwumq_factory_monitor";
	web_id = "jwumq_web";
}

ExternalManage::~ExternalManage()
{

}

bool ExternalManage::Start()
{
	JLOG(INFO) << "ExternalManage start." ;
	if(Setup() != static_cast<int>(RESULT::success))
	{
		return false;
	}
	loop = 1;
//	clear recv_queue
	queue<void *> empty;
	swap(empty, recv_queue);
	
	recv_thread = thread(&ExternalManage::RecvThread, this);
	return true;
}
bool ExternalManage::End()
{
	loop = 0;
	if(lib_jwumq_release != NULL && jmq_handle != NULL)
	{
		lib_jwumq_release(jmq_handle);
	}
	if(lib_handle != NULL)
	{
		lib_jwumq_release(jmq_handle);
	}
	if(lib_handle != NULL)
	{
		dlclose(lib_handle);
	}
	if(recv_thread.joinable())
	{
		recv_thread.join();
	}
	return true;
}

int ExternalManage::Setup()
{
	lib_handle = dlopen(LIB_JWUMQ_NAME, RTLD_NOW);
	if (lib_handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray %s error:%s\n", LIB_JWUMQ_NAME, dlerror());
		return static_cast<int>(RESULT::open_lib_failed);
	}
	lib_version = (LIB_JWUMQ_VERSION)dlsym(lib_handle, "jwumq_version");
	lib_dealer_load = (LIB_JWUMQ_DEALER_LOAD)dlsym(lib_handle, "jwumq_dealer_load");
	lib_jwumq_setup = (LIB_JWUMQ_SETUP)dlsym(lib_handle, "jwumq_setup");
	lib_jwumq_send = (LIB_JWUMQ_SEND)dlsym(lib_handle, "jwumq_send");
	lib_jwumq_release = (LIB_JWUMQ_RELEASE)dlsym(lib_handle, "jwumq_release");
	
	if(lib_version == NULL || lib_dealer_load == NULL || lib_jwumq_setup == NULL || lib_jwumq_send == NULL || lib_jwumq_release == NULL)
	{
		return static_cast<int>(RESULT::not_find_fun_in_lib);
	}
	JLOG(INFO) << LIB_JWUMQ_NAME << " version: " << lib_version();
	
	jmq_handle = lib_dealer_load();
	
		
//		fprintf(stderr, "%s:%s server handle = %ld\n", Time().c_str(), name.c_str(), reinterpret_cast<unsigned long>( server_handle ));

	DEALER_CONF_T conf_t;
	conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	string address = "tcp://*:12120";
	memcpy(conf_t.address, address.c_str(), address.length());
	conf_t.address[address.length()] = 0;
	memcpy(conf_t.identity, self_id.c_str(), self_id.length());
	conf_t.identity[self_id.length()] = 0;
	conf_t.linger = 0;
	conf_t.read_timeout = 5000;
	conf_t.bind = true;
		
	int result = lib_jwumq_setup(jmq_handle, &conf_t, std::bind(&ExternalManage::RecvCallback, this
	, std::placeholders::_1));
	if(result != 0)
	{
		JLOG(INFO) << LIB_JWUMQ_NAME << " setup error, result: " << result;
	}

	return static_cast<int>(RESULT::success);
}

int ExternalManage::RecvCallback(void * msg)
{
	JwumqMessage * recv_msg = new JwumqMessage((JwumqMessage*)msg);
	std::unique_lock<std::mutex> lock(recv_mutex);
	recv_queue.push(recv_msg);
	return static_cast<int>(RESULT::success);
}
void * ExternalManage::ReadMsg()
{
	std::unique_lock<std::mutex> lock(recv_mutex, std::defer_lock);
	if (lock.try_lock() == true)
	{
		if(recv_queue.size() > 0)
		{
			void * recv_msg = recv_queue.front();
			recv_queue.pop();
			return recv_msg;
		}
	}
	return nullptr;
}
void ExternalManage::RecvThread()
{
	JLOG(INFO) << " RecvThread start.";
	while (loop)
	{
		void * p = ReadMsg();
		if(p == nullptr)
		{
			usleep(1000 * 10);
			continue;
		}
		
		JwumqMessage * recv_msg = (JwumqMessage*)p;
		switch(recv_msg->body.command())
		{
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_service_start):
			{
				JLOG(INFO) << " Recv private_service_start, sn: " << recv_msg->body.sn();
				SendHostInfo();
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_request_config):
			{
				JLOG(INFO) << " Recv private_request_config, sn: " << recv_msg->body.sn();
				break;
			}
			case static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_update_config):
			{
				JLOG(INFO) << " Recv private_update_config, sn: " << recv_msg->body.sn();
				SendUpdateConfigAck();
				break;
			}
			default:break;
		}
		web_msg_event(recv_msg);
		delete recv_msg;
	}
	JLOG(INFO) << " RecvThread stop.";
}
void ExternalManage::SendUpdateConfigAck()
{
	string ack = "true";
	unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_update_config_ack, self_id, web_id, ack.c_str(), (int)ack.length()));
	lib_jwumq_send(jmq_handle, msg.get());
}
void ExternalManage::SendConfig(void * config)
{
	if(config == nullptr)
	{
		return ;
	}
	char *cjson_str = NULL;
	MonitorConfig * mc = (MonitorConfig*) config;
	cJSON * root =  cJSON_CreateObject();
	cJSON * jwumq_factory_item =  cJSON_CreateObject();
	
	cJSON_AddItemToObject(root, "jwumq_factory", jwumq_factory_item);
	cJSON_AddItemToObject(jwumq_factory_item, "access_port", cJSON_CreateNumber(mc->factory_config.access_port));
	
	cJSON *pool_array = cJSON_CreateArray();
	for(auto pool : mc->pool_process_list)
	{
		cJSON * pool_item = cJSON_CreateObject();
		cJSON_AddItemToObject(pool_item, "id", cJSON_CreateString(pool.id.c_str()));
		cJSON_AddItemToObject(pool_item, "name", cJSON_CreateString(pool.name.c_str()));
		cJSON_AddItemToObject(pool_item, "enable", cJSON_CreateBool(pool.enable));
		cJSON_AddItemToObject(pool_item, "access_port", cJSON_CreateNumber(pool.access_port));
		cJSON_AddItemToObject(pool_item, "max_queue_counts", cJSON_CreateNumber(pool.max_queue_counts));
		cJSON_AddItemToArray(pool_array, pool_item);
	}
	cJSON_AddItemToObject(root, "pool_list", pool_array);
	cjson_str = cJSON_Print(root);
	
	unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_request_config_ack, self_id, web_id, cjson_str, (int)strlen(cjson_str)));
	int result = lib_jwumq_send(jmq_handle, msg.get());
	
	JLOG(INFO) << "Send private_request_config_ack, result:" << result
				<< ", length: " << strlen(cjson_str)
				<< ", content: " << cjson_str;
	
	free(cjson_str);
	cJSON_Delete(root);
}

void ExternalManage::SendHostInfo()
{
	string info = "host info";
	char host_name[64];
	char *cjson_str = NULL;
	
    cJSON * root =  cJSON_CreateObject();
	gethostname(host_name, sizeof(host_name));
	
	cJSON_AddItemToObject(root, "host_name", cJSON_CreateString(host_name));
	
	cJSON *info_array = cJSON_CreateArray();
	
	struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);
	while (ifAddrStruct!=NULL)
	{
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET)
		{ // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
//            printf("%s IP Address %s/n", ifAddrStruct->ifa_name, addressBuffer);
			cJSON * info = cJSON_CreateObject();
			cJSON_AddItemToObject(info, "eth", cJSON_CreateString(ifAddrStruct->ifa_name));
			cJSON_AddItemToObject(info, "ip", cJSON_CreateString(addressBuffer));
			cJSON_AddItemToArray(info_array, info);
        }
		else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6)
		{ // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
//            printf("%s IP Address %s/n", ifAddrStruct->ifa_name, addressBuffer);
			cJSON * info = cJSON_CreateObject();
			cJSON_AddItemToObject(info, "eth", cJSON_CreateString(ifAddrStruct->ifa_name));
			cJSON_AddItemToObject(info, "ip", cJSON_CreateString(addressBuffer));
			cJSON_AddItemToArray(info_array, info);
        }
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
	cJSON_AddItemToObject(root, "host_infos", info_array);
	cjson_str = cJSON_Print(root);
	
	unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_service_start_ack, self_id, web_id, cjson_str, (int)strlen(cjson_str)));
	int result = lib_jwumq_send(jmq_handle, msg.get());
	JLOG(INFO) << "Send private_service_start_ack, result:" << result << ", content: " << cjson_str;
	free(cjson_str);
	cJSON_Delete(root);
}



