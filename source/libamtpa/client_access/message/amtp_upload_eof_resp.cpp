//
//  amtp_upload_eof_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_upload_eof_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_upload_eof_resp::amtp_upload_eof_resp()
{
	
}

amtp_upload_eof_resp::~amtp_upload_eof_resp()
{
	
}

UPLOAD_EOF_RESP_STRU amtp_upload_eof_resp::data(JwumqMessage * msg)
{
	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.ParseFromArray(msg->RawData(),msg->RawDataLen());
	
	amtpap::UploadEofRespV1 resp;
	resp.ParseFromArray(cmdprimitive.payload_data().c_str(), cmdprimitive.payload_data().size());
	
	UPLOAD_EOF_RESP_STRU s;
	memset(&s, 0, sizeof(UPLOAD_EOF_RESP_STRU));
	int length = ((int)resp.file_name().length() >= FILENAME_LENGTH) ? FILENAME_LENGTH : (int)resp.file_name().length();
	memcpy(s.file_name, resp.file_name().c_str(), length);
	s.packet_count = resp.packet_count();
	s.file_id = resp.file_id();
	s.result = resp.result();
	s.module = resp.module();
	
	int token_len = (int)(TOKEN_BUFFER_SIZE >= resp.token().length()) ? (int)resp.token().length() : TOKEN_BUFFER_SIZE;
	memset(_token, 0, TOKEN_BUFFER_SIZE);
	memcpy(_token, resp.token().c_str(), token_len);
	return s;
}
void amtp_upload_eof_resp::token(unsigned char *token_buf)
{
	if(token_buf != NULL)
	{
		memcpy(token_buf, _token, TOKEN_BUFFER_SIZE);
	}
}


