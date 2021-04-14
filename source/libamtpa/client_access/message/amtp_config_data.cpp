//
//  amtp_config_data.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/4.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_config_data.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_config_data::amtp_config_data()
{
	
}

amtp_config_data::~amtp_config_data()
{
	
}

bool amtp_config_data::data(JwumqMessage * msg, CONFIG_DATA_STRU *s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::ConfigDataV1 req;
	req.ParseFromArray(cmdprimitive.payload_data().c_str(), cmdprimitive.payload_data().size());

	memset(s, 0, sizeof(STATUS_INFO_RESP_STRU));
//	s->version = req.version();
	int atuid_version_length = (req.atuid_version().length() >= sizeof(s->atuid_version)) ? (int)sizeof(s->atuid_version) : (int)req.atuid_version().length();
	memcpy(s->atuid_version, req.atuid_version().c_str(), atuid_version_length);
	s->packet_count = req.packet_count();
	s->packet_no = req.packet_no();
	s->data_length = req.data_length();

	if(s->data_length > 1024 * 1024 || s->data_length <= 0)
	{
		return false;
	}
	s->config_data = new char[s->data_length];
	memcpy(s->config_data, req.config_data().c_str(), s->data_length);

	int token_len = (TOKEN_BUFFER_SIZE >= req.token().length()) ? (int)req.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, req.token().c_str(), token_len);
	return true;
}
void amtp_config_data::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}
