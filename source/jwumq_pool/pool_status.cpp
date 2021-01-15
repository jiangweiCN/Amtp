//
//  pool_status.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "pool_status.hpp"
#include "../jlog/jLog.h"


PoolStatus::PoolStatus()
{
	memset(&pool_status, 0, sizeof(POOL_STATUS_T));
}

PoolStatus::~PoolStatus()
{

}
//
//void PoolStatus::GetStatus(jwumq_pb_msg::PoolStatusReport & pb)
//{
////	pb.set_private_bytes(1);
////	pb.set_private_count(2);
////	pb.set_queue_count(3);
////
////	for(int x = 0; x < 3; x++)
////	{
////		jwumq_pb_msg::QueueStatus* qs;
////		qs = pb.add_queue_status();
////		qs->set_name("123");
////		qs->set_total_bytes(789);
////		qs->set_package_count(654);
////		qs->set_flow(45.6f);
////	}
//}
void PoolStatus::Reset(jwumq_pb_msg::PoolStatusReport & pb)
{
	std::unique_lock<std::mutex> pool_status_lock(pool_status_mutex, std::defer_lock);
	pool_status_lock.lock();
	
	pb.set_private_bytes(pool_status.private_bytes);
	pb.set_private_count(pool_status.private_count);
	pb.set_queue_count(pool_status.queue_count);
	
	JLOG(INFO) << "Pool status, private_bytes: " << pool_status.private_bytes
				<< ", private_coun: " << pool_status.private_count
				<< ", queue_count: " << pool_status.queue_count;
	memset(&pool_status, 0, sizeof(POOL_STATUS_T));
	
	pool_status_lock.unlock();
	
	std::unique_lock<std::mutex> queue_status_lock(queue_status_mutex, std::defer_lock);
	queue_status_lock.lock();
	
	for(auto &queue : queue_status)
	{
		string q_name = queue.first;
		QUEUE_STATUS_T q_status = queue.second;
		time_t c_t = time(NULL);
		
		jwumq_pb_msg::QueueStatus* qs;
		qs = pb.add_queue_status();
		qs->set_name(q_name);
		qs->set_total_bytes(q_status.total_bytes);
		qs->set_package_count(q_status.package_count);
		float flow = (float)q_status.total_bytes /(float)(c_t - q_status.last_time);
		qs->set_flow(flow);
		
		JLOG(INFO) << "Queue status, name: " << q_name.c_str()
			<< ", total_bytes: " << q_status.total_bytes
			<< ", package_count: " << q_status.package_count
			<< ", flow: " << flow;
		q_status.total_bytes = 0;
		q_status.package_count = 0;
		q_status.last_time = c_t;
	}
	
	queue_status_lock.unlock();
}

void PoolStatus::UpdatePoolPrivate(uint32_t bytes, uint32_t count)
{
	std::unique_lock<std::mutex> lock(pool_status_mutex);
	pool_status.private_bytes += bytes;
	pool_status.private_count += count;
	
}



