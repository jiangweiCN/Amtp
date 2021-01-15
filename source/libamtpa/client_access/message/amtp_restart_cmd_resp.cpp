//
//  amtp_restart_cmd_resp.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_restart_cmd_resp.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_restart_cmd_resp::amtp_restart_cmd_resp()
{
	
}

amtp_restart_cmd_resp::~amtp_restart_cmd_resp()
{
	
}

unique_ptr<JwumqMessage> amtp_restart_cmd_resp::data(RESTART_CMD_RESP_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::ReatartRespV1 resp;
	resp.set_token((char*)token);
	resp.set_result(s->result);
	
	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(sn);
	cmdprimitive.set_msg_id(amtpap::MsgID::RESTART_CMD_RESP_V1);
	cmdprimitive.set_direction(1);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, src_id, des_id, send_buf.get(), (int)length);
//	strcpy(msg->body.src_id, license_info.jwumq_id().c_str());
	msg->body.set_src_id(load_conf.jwumq_id);
	
	return msg;
}

