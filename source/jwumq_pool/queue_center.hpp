//
//  queue_center.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef queue_center_hpp
#define queue_center_hpp

#include <stdio.h>
#include <map>
#include "pool_status.hpp"
#include "access_unit.hpp"
#include "queue/queue_base.hpp"


using QUEUE_NAME = string;
using QUEUE_INFO_MAP = map<QUEUE_NAME, shared_ptr<QueueBase>>;

class QueueCenter
{
public:
	QueueCenter(string id, uint32_t port, shared_ptr<PoolStatus> status);
	~QueueCenter(void);
	
	bool Start();
	bool End();
	
	void AssignedQueueListener(string);
	void CreatePartPipeListener(string, string, int &);
	void CreatePartMemberListener(string, string, int &);
	void CreatePartProducerListener(string, string, int &);
	void CreatePartConsumerListener(string, string, int &);
	void RecvPublicTextListener(shared_ptr<JwumqMessage>);
	void RecvPublicBytesListener(shared_ptr<JwumqMessage>);
private:
	bool FindQueue(string);
public:

private:
	string pool_id;
	uint32_t access_port;
	shared_ptr<PoolStatus> status;
	shared_ptr<AccessUnit> access_unit;
	mutex queue_info_mutex;
	
	QUEUE_INFO_MAP queue_info_map;
};


#endif /* queue_center_hpp */
