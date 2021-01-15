//
//  queue_member.hpp
//  Amtp
//
//  Created by jiangwei on 2020/7/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef queue_member_hpp
#define queue_member_hpp

#include <stdio.h>
#include <vector>
#include "queue_base.hpp"
#include "../access_unit.hpp"


class QueueMember : public QueueBase
{
public:
	QueueMember(string queue_name);
	virtual ~QueueMember(void);
	
	int AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type);
	int SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);
	int SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>);

private:
	vector<string> GetDesNode(string src, string des);
public:

	
private:
	
};

#endif /* queue_member_hpp */
