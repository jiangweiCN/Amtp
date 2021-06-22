//
//  queue_base.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef queue_base_hpp
#define queue_base_hpp

#include <stdio.h>
#include <string>
#include "../pool_def.h"
#include "../../jwumq/jwumq_message.hpp"
#include "../access_unit.hpp"



constexpr int MAX_PART_PIPE_COUNT = 2;
constexpr int MAX_PART_MEMBER_COUNT = 1000;
enum class JWUMQ_PART_TYPE_ENUM : uint16_t
{
	pipe = 0x01,
	member,
	producer,
	consumer,
};

class QueueBase
{
public:
	QueueBase();
	virtual ~QueueBase(void);
	
	virtual int AddPart(string node_id, JWUMQ_PART_TYPE_ENUM type) = 0;
	virtual int SendText(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>) = 0;
	virtual int SendBytes(shared_ptr<AccessUnit> conn, shared_ptr<JwumqMessage>) = 0;

protected:
	string PartName();
public:

protected:
	string queue_name;
	int part_count;
	QUEUE_PART_INFO_MAP part_info;
	mutex part_info_mutex;
private:
	
	
};

#endif /* queue_base_hpp */
