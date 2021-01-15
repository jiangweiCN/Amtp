//
//  queue_producer.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef queue_producer_hpp
#define queue_producer_hpp

#include <stdio.h>
#include "queue_base.hpp"
#include "../access_unit.hpp"


class QueueProducer : public QueueBase
{
public:
	QueueProducer(string queue_name);
	virtual ~QueueProducer(void);
	
	int AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type);
	int SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);
	int SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);

private:
	string GetDesNode(string src);
public:

	
private:
	
};


#endif /* queue_producer_hpp */
