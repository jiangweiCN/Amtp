//
//  libjwumqa_part_consumer.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_part_consumer_hpp
#define libjwumqa_part_consumer_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "pool_connection.hpp"


class JwumqPartConsumerImpl:public JwumqPartConsumer
{
public:
	JwumqPartConsumerImpl(string url, string queue_name, string node_id, uint64_t session);
	~JwumqPartConsumerImpl(void);
	
	int Send(JMessage *);
	JMessage * Receive(int timeout);
	int Setup();
	bool Available();
private:
	void PartConsumerRespListener(uint32_t);
	void Release();
private:
	string url;
	string queue_name;
	string node_id;
	uint64_t session;
	bool available;
	
	unique_ptr<PoolConnection> conn;
};


#endif /* libjwumqa_part_consumer_hpp */
