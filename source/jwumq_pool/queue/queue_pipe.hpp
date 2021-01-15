//
//  queue_pipe.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/24.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef queue_pipe_hpp
#define queue_pipe_hpp

#include <stdio.h>
#include "queue_base.hpp"
#include "../access_unit.hpp"


class QueuePipe : public QueueBase
{
public:
	QueuePipe(string queue_name);
	virtual ~QueuePipe(void);
	
	int AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type);
	int SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);
	int SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);

private:
	string GetDesNode(string src);
public:

	
private:
	
};

#endif /* queue_pipe_hpp */
