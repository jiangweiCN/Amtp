//
//  amtp_restart_cmd.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_restart_cmd.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_restart_cmd::amtp_restart_cmd()
{
	
}

amtp_restart_cmd::~amtp_restart_cmd()
{
	
}

bool amtp_restart_cmd::data(JwumqMessage * msg, RESTART_CMD_STRU *s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::ReatartCmdV1 req;
	req.ParseFromArray(cmdprimitive.payload_data().c_str(), cmdprimitive.payload_data().size());

	memset(s, 0, sizeof(RESTART_CMD_STRU));
	s->ts_sec = req.ts_sec();
	s->ts_usec = req.ts_usec();

	int token_len = (TOKEN_BUFFER_SIZE >= req.token().length()) ? req.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, req.token().c_str(), token_len);
	return true;
}
void amtp_restart_cmd::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}

