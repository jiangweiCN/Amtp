//
//  amtp_config_notify.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_config_notify.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_config_notify::amtp_config_notify()
{
	
}

amtp_config_notify::~amtp_config_notify()
{
	
}

bool amtp_config_notify::data(JwumqMessage * msg, CONFIG_NOTIFY_STRU *s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::ConfigNotifyV1 req;
	req.ParseFromArray(cmdprimitive.payload_data().c_str(), (int)cmdprimitive.payload_data().size());

	memset(s, 0, sizeof(STATUS_INFO_RESP_STRU));
//	s->version = req.version();
	s->packet_count = req.packet_count();
	int length = ((int)req.md5().length() >= 32) ? 32 : (int)req.md5().length();
	memcpy(s->md5, req.md5().c_str(), length);
	
	int atuid_version_length = (req.atuid_version().length() >= sizeof(s->atuid_version)) ? (int)sizeof(s->atuid_version) : (int)req.atuid_version().length();
	memcpy(s->atuid_version, req.atuid_version().c_str(), atuid_version_length);

	int token_len = (TOKEN_BUFFER_SIZE >= req.token().length()) ? (int)req.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, req.token().c_str(), token_len);
	return true;
}
void amtp_config_notify::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}
