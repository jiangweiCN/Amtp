//
//  queue_base.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "queue_base.hpp"


QueueBase::QueueBase()
{
	
}

QueueBase::~QueueBase()
{
	
}

string QueueBase::PartName()
{
	string part_name;
	for(auto &v : part_info)
	{
		part_name += v.first + ", ";
	}
	return part_name;
}
