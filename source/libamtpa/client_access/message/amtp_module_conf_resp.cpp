//
//  amtp_module_conf_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/25.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_module_conf_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_module_conf_resp::amtp_module_conf_resp()
{
	
}

amtp_module_conf_resp::~amtp_module_conf_resp()
{
	
}

void amtp_module_conf_resp::data(JwumqMessage *msg, MODULE_CONF_RESP_STRU &s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::ModuleConfRespV1 resp;
	resp.ParseFromArray(cmdprimitive.payload_data().c_str(), (int)cmdprimitive.payload_data().size());
	
	// MODULE_CONF_RESP_STRU s;
	memset(&s, 0, sizeof(MODULE_CONF_RESP_STRU));
	s.result = resp.result();
	int token_len = (TOKEN_BUFFER_SIZE >= resp.token().length()) ? (int)resp.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, resp.token().c_str(), token_len);
	// return s;
}
void amtp_module_conf_resp::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}

