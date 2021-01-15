//
//  libjwumqa_part_pipe.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_part_pipe_hpp
#define libjwumqa_part_pipe_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "pool_connection.hpp"


class JwumqPartPipeImpl:public JwumqPartPipe
{
public:
	JwumqPartPipeImpl(string url, string queue_name, string node_id, uint64_t session);
	~JwumqPartPipeImpl(void);
	
	int Send(JMessage *);
	JMessage * Receive(int timeout);
	int Setup();
	bool Available();
private:
	void PartPipeRespListener(uint32_t);
	void Release();
private:
	string url;
	string queue_name;
	string node_id;
	uint64_t session;
	bool available;
	
	unique_ptr<PoolConnection> conn;
};

#endif /* libjwumqa_part_pipe_hpp */
