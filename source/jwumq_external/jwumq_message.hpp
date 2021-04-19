//
//  jwumq_message.hpp
//  Amtp
//
//  Created by jiangwei on 2020/7/24.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_message_hpp
#define jwumq_message_hpp

#include <stdio.h>
#include <stdint.h>
#include <string>
#include "jwumq_define.h"
#include "jwumq_external_message.pb.h"

using namespace std;

class JwumqMessage
{
public:
	JwumqMessage(const void * msg_data, int msg_data_len);
	JwumqMessage(JWUMQ_COMMAND_ENUM command, string src_id, string des_id, const void * raw_data, int raw_data_len);
	JwumqMessage(JWUMQ_COMMAND_ENUM command, string src_id, string des_id, string queue_name, const void * raw_data, int raw_data_len);
	JwumqMessage(JWUMQ_COMMAND_ENUM command, const void * raw_data, int raw_data_len);
	JwumqMessage();
	JwumqMessage(JwumqMessage *);
	~JwumqMessage(void);
	
	int MsgDataLen();
	int RawDataLen();
	void * RawData();
	void * MsgData();
	void SetDestination(string des_id);
	string Print();
public:
	jwumq_external_message_pb::jwumq_external_message body;

private:
	uint32_t msg_data_len;
	unique_ptr<unsigned char[]> msg_data;
};


#endif /* jwumq_message_hpp */
