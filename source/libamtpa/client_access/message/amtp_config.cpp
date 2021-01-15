//
//  amtp_config.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_config.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_config::amtp_config()
{
	
}

amtp_config::~amtp_config()
{
	
}

unique_ptr<JwumqMessage> amtp_config::data(CONFIG_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::ConfigV1 req;
	req.set_token((char*)token);
//	req.set_version(s->version);
	req.set_atuid_version(s->atuid_version);
	
	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(), (int)req_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(sn);
	cmdprimitive.set_msg_id(amtpap::MsgID::CONFIG_V1);
	cmdprimitive.set_direction(1);
	cmdprimitive.set_payload_data(req_buf.get(), req_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, src_id, des_id, send_buf.get(), (int)length);
//	strcpy(msg->body.src_id, license_info.jwumq_id().c_str());
	msg->body.set_src_id(load_conf.jwumq_id);
	
	return msg;
}
