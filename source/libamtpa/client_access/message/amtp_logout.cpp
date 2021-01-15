//
//  amtp_logout.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_logout.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"


amtp_logout::amtp_logout()
{
	
}

amtp_logout::~amtp_logout()
{
	
}

unique_ptr<JwumqMessage> amtp_logout::data(unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::LogoutV1 logout;
	logout.set_token((char*)token);
	
	size_t logout_len = logout.ByteSizeLong();
	unique_ptr<char[]> logout_buf(new char[logout_len]());
	logout.SerializeToArray(logout_buf.get(), (int)logout_len);


	amtpap::CmdPrimitive req;
	req.set_type(amtpap::MsgType::PUBLIC);
	req.set_serial_number(sn);
	req.set_msg_id(amtpap::MsgID::LOGOUT_V1);
	req.set_direction(1);
	req.set_payload_data(logout_buf.get(), logout_len);

	size_t length = req.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	req.SerializeToArray(send_buf.get(), (int)length);

	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, src_id, des_id, send_buf.get(), (int)length);
//	strcpy(msg->body.src_id, license_info.jwumq_id().c_str());
	msg->body.set_src_id(load_conf.jwumq_id);
	return msg;
}

