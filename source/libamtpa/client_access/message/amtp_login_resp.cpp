//
//  amtp_login_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/1.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_login_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"


amtp_login_resp::amtp_login_resp()
{
	
}

amtp_login_resp::~amtp_login_resp()
{
	
}

void amtp_login_resp::data(JwumqMessage *msg, LOGIN_RESP_STRU &s)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::LoginRespV1 resp;
	resp.ParseFromArray(cmdprimitive.payload_data().c_str(), (int)cmdprimitive.payload_data().size());
	
	memset(&s, 0, sizeof(LOGIN_RESP_STRU));
	s.result = resp.result();
	int token_len = (int)resp.token().length();
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, resp.token().c_str(), token_len);
	
	int manufacturer_len = (resp.manufactor().length() < sizeof(s.manufacturer))?(int)resp.manufactor().length():sizeof(s.manufacturer);
	memset(s.manufacturer, 0, sizeof(s.manufacturer));
	memcpy(s.manufacturer, resp.manufactor().c_str(), manufacturer_len);
}
void amtp_login_resp::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}
