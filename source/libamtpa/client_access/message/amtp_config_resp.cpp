//
//  amtp_config_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_config_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_config_resp::amtp_config_resp()
{
	
}

amtp_config_resp::~amtp_config_resp()
{
	
}

void amtp_config_resp::data(JwumqMessage *msg, CONFIG_RESP_STRU &s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::ConfigRespV1 resp;
	resp.ParseFromArray(cmdprimitive.payload_data().c_str(), cmdprimitive.payload_data().size());
	
	// CONFIG_RESP_STRU s;
	memset(&s, 0, sizeof(CONFIG_RESP_STRU));
//	s.version = resp.version();
	int atuid_version_length = (resp.atuid_version().length() >= sizeof(s.atuid_version)) ? (int)sizeof(s.atuid_version) : (int)resp.atuid_version().length();
	memcpy(s.atuid_version, resp.atuid_version().c_str(), atuid_version_length);
	
	s.result = resp.result();
	s.update = resp.update();
	s.packet_count = resp.packet_count();
	int length = (resp.md5().length() >= 32) ? 32 : (int)resp.md5().length();
	memcpy(s.md5, resp.md5().c_str(), length);
	int token_len = (TOKEN_BUFFER_SIZE >= resp.token().length()) ? (int)resp.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, resp.token().c_str(), token_len);
	// return s;
}
void amtp_config_resp::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}

