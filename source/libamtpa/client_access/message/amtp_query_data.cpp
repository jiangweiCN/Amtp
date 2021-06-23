//
//  amtp_query_data.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_query_data.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_query_data::amtp_query_data()
{
	
}

amtp_query_data::~amtp_query_data()
{
	
}

bool amtp_query_data::data(JwumqMessage * msg, QUERY_DATA_STRU *s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::QueryDataV1 req;
	req.ParseFromArray(cmdprimitive.payload_data().c_str(), (int)cmdprimitive.payload_data().size());

	memset(s, 0, sizeof(QUERY_DATA_STRU));
	int length = (FILENAME_LENGTH >= req.file_name().length()) ? (int)req.file_name().length() : TOKEN_BUFFER_SIZE;
	memset(s, 0, TOKEN_BUFFER_SIZE);
	memcpy(s->file_name, req.file_name().c_str(), length);
	s->file_id = req.file_id();
	s->packet_no = req.packet_no();

	int token_len = (TOKEN_BUFFER_SIZE >= req.token().length()) ? (int)req.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, req.token().c_str(), token_len);
	return true;
}
void amtp_query_data::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}
