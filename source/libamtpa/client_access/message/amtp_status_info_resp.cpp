//
//  amtp_status_info_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_status_info_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_status_info_resp::amtp_status_info_resp()
{
	
}

amtp_status_info_resp::~amtp_status_info_resp()
{
	
}

STATUS_INFO_RESP_STRU amtp_status_info_resp::data(JwumqMessage * msg)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::StatusInfoRespV1 resp;
	resp.ParseFromArray(cmdprimitive.payload_data().c_str(), (int)cmdprimitive.payload_data().size());
	
	STATUS_INFO_RESP_STRU s;
	memset(&s, 0, sizeof(STATUS_INFO_RESP_STRU));
	s.result = resp.result();
	int token_len = (TOKEN_BUFFER_SIZE >= resp.token().length()) ? (int)resp.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, resp.token().c_str(), token_len);
	return s;
}
void amtp_status_info_resp::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}

