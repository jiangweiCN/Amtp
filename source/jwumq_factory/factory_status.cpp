//
//  factory_status.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "factory_status.hpp"
#include <iomanip>
#include "../jlog/jLog.h"
#include "../jwumq_protocol/jwumq_msg.pb.h"



FactoryStatus::FactoryStatus()
{
	
}

FactoryStatus::~FactoryStatus()
{
	
}

void FactoryStatus::PoolStatusListener(shared_ptr<JwumqMessage> recv_msg)
{
	shared_ptr<JwumqMessage> msg = recv_msg;
	string pool_id = msg->body.src_id();
	jwumq_pb_msg::PoolStatusReport report;
	report.ParseFromArray(msg->RawData(), msg->RawDataLen());
	string pool_name = report.name();
//	uint32_t sn = report.sn();
//	uint32_t private_bytes = report.private_bytes();
//	uint32_t private_count = report.private_count();
	uint32_t access_port = report.access_port();
	uint64_t report_time = report.report_time();
	uint32_t queue_count = report.queue_count();
	
	UpdatePoolInfo(pool_name, access_port);
	uint32_t bytes = 0;
	uint32_t packages = 0;
	
	for(uint32_t i = 0; i < queue_count; i++)
	{
		jwumq_pb_msg::QueueStatus q = report.queue_status(i);
		string queue_name = q.name();
		uint32_t package_count = q.package_count();
		uint32_t total_bytes = q.total_bytes();
		
		UpdateQueueInfo(pool_name, queue_name, access_port);
		UpdateQueueStatus(queue_name, total_bytes, package_count, report_time);
		
		bytes += total_bytes;
		packages += package_count;
	}
	
	UpdatePoolStatus(pool_name, bytes, packages, queue_count, report_time);
	
}

void FactoryStatus::UpdateQueueInfo(string pool_name, string queue_name, uint32_t access_port)
{
	std::unique_lock<std::mutex> lock(queue_info_mutex);
	if(queue_info_map.find(queue_name) == queue_info_map.end())
	{
		QUEUE_INFO_T qi = {pool_name, queue_name, access_port};
		queue_info_map[qi.queue_name] = qi;
		JLOG(INFO) << "Insert Queue info! pool name:" << qi.pool_name.c_str()
		<< ", queue name:" << qi.queue_name.c_str()
		<< ", access port:" << qi.access_port;
	}
}

void FactoryStatus::UpdateQueueStatus(string queue_name, uint32_t bytes, uint32_t packages, uint64_t report_time)
{
	std::unique_lock<std::mutex> lock(queue_status_mutex);
	if(queue_status_map.find(queue_name) == queue_status_map.end())
	{
		float interval = QUERY_STATUS_INTERVAL_SEC;
		float flow = bytes / interval;
		QUEUE_STATUS_T qs = {queue_name, bytes, packages, bytes, packages, report_time, flow};
		queue_status_map[qs.queue_name] = qs;
		
		std::time_t t(qs.report_time);
		JLOG(INFO) << "Insert queue status! queue name:" << queue_name.c_str()
		<< ", bytes:" << qs.bytes
		<< ", packages:" << qs.packages
		<< ", bytes sum:" << qs.bytes_sum
		<< ", package sum:" << qs.package_sum
		<< ", report sum:" << put_time(std::localtime(&t), "[%F %T]")
		<< ", flow:" << qs.flow;
	}
	else
	{
		queue_status_map[queue_name].bytes = bytes;
		queue_status_map[queue_name].packages = packages;
		queue_status_map[queue_name].bytes_sum += bytes;
		queue_status_map[queue_name].package_sum += packages;
		
		uint64_t last_time = queue_status_map[queue_name].report_time;
		float interval = (float)(report_time - last_time);
		if(interval == 0.0f)
		{
			interval = QUERY_STATUS_INTERVAL_SEC;
		}
		float flow = bytes / interval;
		
		queue_status_map[queue_name].report_time = report_time;
		queue_status_map[queue_name].flow = flow;
		
		QUEUE_STATUS_T qs = queue_status_map[queue_name];
		std::time_t t(qs.report_time);
		JLOG(INFO) << "Update queue status! queue name:" << queue_name.c_str()
		<< ", bytes:" << qs.bytes
		<< ", packages:" << qs.packages
		<< ", bytes sum:" << qs.bytes_sum
		<< ", package sum:" << qs.package_sum
		<< ", report sum:" << put_time(std::localtime(&t), "[%F %T]")
		<< ", flow:" << qs.flow;
	}
}

void FactoryStatus::UpdatePoolInfo(string pool_name, uint32_t access_port)
{
	std::unique_lock<std::mutex> lock(pool_info_mutex);
	if(pool_info_map.find(pool_name) == pool_info_map.end())
	{
		POOL_INFO_T pi = {pool_name, access_port};
		pool_info_map[pool_name] = pi;
		JLOG(INFO) << "Insert pool info! pool name:" << pi.name.c_str()
		<< ", access port:" << pi.access_port;
	}
}

void FactoryStatus::UpdatePoolStatus(string pool_name, uint32_t bytes, uint32_t packages, uint32_t queues, uint64_t report_time)
{
	std::unique_lock<std::mutex> lock(pool_status_mutex);
	if(pool_status_map.find(pool_name) == pool_status_map.end())
	{
		float interval = QUERY_STATUS_INTERVAL_SEC;
		float flow = bytes / interval;
		POOL_STATUS_T ps = {pool_name, bytes, packages, bytes, packages, queues, report_time, flow};
		pool_status_map[pool_name] = ps;
		
		std::time_t t(ps.report_time);
		JLOG(INFO) << "Insert pool status! pool name:" << ps.name.c_str()
		<< ", bytes:" << ps.bytes
		<< ", packages:" << ps.packages
		<< ", bytes sum:" << ps.bytes_sum
		<< ", package sum:" << ps.package_sum
		<< ", queue sum:" << ps.queue_sum
		<< ", report sum:" << put_time(std::localtime(&t), "[%F %T]")
		<< ", flow:" << ps.flow;
	}
	else
	{
		pool_status_map[pool_name].bytes = bytes;
		pool_status_map[pool_name].packages = packages;
		pool_status_map[pool_name].bytes_sum += bytes;
		pool_status_map[pool_name].package_sum += packages;
		pool_status_map[pool_name].queue_sum = queues;
		
		uint64_t last_time = pool_status_map[pool_name].report_time;
		float interval = (float)(report_time - last_time);
		if(interval == 0.0f)
		{
			interval = QUERY_STATUS_INTERVAL_SEC;
		}
		float flow = bytes / interval;
		
		pool_status_map[pool_name].report_time = report_time;
		pool_status_map[pool_name].flow = flow;
		
		POOL_STATUS_T ps = pool_status_map[pool_name];
		std::time_t t(ps.report_time);
		JLOG(INFO) << "Update pool status! pool name:" << ps.name.c_str()
		<< ", bytes:" << ps.bytes
		<< ", packages:" << ps.packages
		<< ", bytes sum:" << ps.bytes_sum
		<< ", package sum:" << ps.package_sum
		<< ", report sum:" << put_time(std::localtime(&t), "[%F %T]")
		<< ", flow:" << ps.flow;
	}
}

void FactoryStatus::GetAvailablePool(string queue_name, string &pool_name, int &access_port)
{
	shared_ptr<QUEUE_INFO_T> queue_info = FindQueue(queue_name);
	if(queue_info == nullptr)
	{
		pool_name = GetMinQueueSumPool();
		access_port = GetPoolAccessPort(pool_name);
		JLOG(INFO) << "Queue created! pool name:" << pool_name.c_str()
		<< ", queue name:" << queue_name.c_str()
		<< ", access port:" << access_port;
		UpdateQueueInfo(pool_name, queue_name, access_port);
	}
	else
	{
		access_port = queue_info->access_port;
		pool_name = queue_info->pool_name;
		JLOG(INFO) << "Queue is exist! pool name:" << pool_name.c_str()
		<< ", queue name:" << queue_name.c_str()
		<< ", access port:" << access_port;
	}
}

shared_ptr<QUEUE_INFO_T> FactoryStatus::FindQueue(string queue_name)
{
	shared_ptr<QUEUE_INFO_T> queue_info = nullptr;
	
	std::unique_lock<std::mutex> lock(queue_info_mutex);
	QUEUE_INFO_MAP::iterator it;
	it = queue_info_map.find(queue_name);
	if(it != queue_info_map.end())
	{
		QUEUE_INFO_T info = it->second;
		queue_info = make_shared<QUEUE_INFO_T>(info);
	}
	return queue_info;
}

string FactoryStatus::GetMinQueueSumPool()
{
	int queue_sum = -1;
	string pool_name;
	std::unique_lock<std::mutex> lock(pool_status_mutex);
	for(auto &p : pool_status_map)
	{
		POOL_STATUS_T ps = p.second;
		if((int)ps.queue_sum > queue_sum)
		{
			queue_sum = ps.queue_sum;
			pool_name = ps.name;
		}
	}
	return pool_name;
}

uint32_t FactoryStatus::GetPoolAccessPort(string pool_name)
{
	uint32_t access_port = 0;
	std::unique_lock<std::mutex> lock1(pool_info_mutex);
	if(pool_info_map.find(pool_name) != pool_info_map.end())
	{
		POOL_INFO_T pi = pool_info_map[pool_name];
		access_port = pi.access_port;
	}
	return access_port;
}



