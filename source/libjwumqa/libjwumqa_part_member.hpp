//
//  libjwumqa_part_member.hpp
//  Amtp
//
//  Created by jiangwei on 2020/7/23.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_part_member_hpp
#define libjwumqa_part_member_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "pool_connection.hpp"


class JwumqPartMemberImpl:public JwumqPartMember
{
public:
	JwumqPartMemberImpl(string url, string queue_name, string node_id, uint64_t session);
	~JwumqPartMemberImpl(void);
	
	int Send(JMessage *);
	JMessage * Receive(int timeout);
	int Setup();
	bool Available();
private:
	void PartMemberRespListener(uint32_t);
	void Release();
private:
	string url;
	string queue_name;
	string node_id;
	uint64_t session;
	bool available;
	
	unique_ptr<PoolConnection> conn;
};

#endif /* libjwumqa_part_member_hpp */
