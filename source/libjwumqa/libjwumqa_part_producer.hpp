//
//  libjwumqa_part_producer.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/10.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_part_producer_hpp
#define libjwumqa_part_producer_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "pool_connection.hpp"


class JwumqPartProducerImpl:public JwumqPartProducer
{
public:
	JwumqPartProducerImpl(string url, string queue_name, string node_id, uint64_t session);
	~JwumqPartProducerImpl(void);
	
	int Send(JMessage *);
	JMessage * Receive(int timeout);
	int Setup();
	bool Available();
private:
	void PartProducerRespListener(uint32_t);
	void Release();
private:
	string url;
	string queue_name;
	string node_id;
	uint64_t session;
	bool available;
	
	unique_ptr<PoolConnection> conn;
};


#endif /* libjwumqa_part_producer_hpp */
