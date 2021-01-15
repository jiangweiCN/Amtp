//
//  libjwumqa_destination.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/18.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_destination_hpp
#define libjwumqa_destination_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "factory_connection.hpp"


class JwumqDestinationImpl:public JwumqDestination
{
public:
	JwumqDestinationImpl(string queue_name, uint64_t session);
	~JwumqDestinationImpl(void);
	
	JwumqPartPipe* CreatePartPipe(string node_id);
	JwumqPartMember* CreatePartMember(string node_id);
	JwumqPartProducer* CreatePartProducer(string node_id);
	JwumqPartConsumer* CreatePartConsumer(string node_id);
	void CreateQueueRespListener(bool availableint, int queue_port);
	bool Available();
	void DestroyPart(JwumqPart*);
public:
	shared_ptr<FactoryConnection> factory_conn;
private:
	string queue_name;
	uint64_t session;
	string queue_url;
	bool available;
};

#endif /* libjwumqa_destination_hpp */
