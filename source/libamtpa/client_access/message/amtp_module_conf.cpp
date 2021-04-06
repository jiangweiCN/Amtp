//
//  amtp_module_conf.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/25.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_module_conf.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_module_conf::amtp_module_conf()
{
	
}

amtp_module_conf::~amtp_module_conf()
{
	
}

unique_ptr<JwumqMessage> amtp_module_conf::data(MODULE_CONF_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::ModuleConfV1 req;
	req.set_token((char*)token);
	req.set_module_num(s->module_num);
	req.set_module_type(static_cast<uint32_t>(s->module_type));
	req.set_chip_manufacturer(static_cast<uint32_t>(s->chip_manufacturer));
	req.set_chip_type(s->chip_type);
	req.set_icd_version(s->icd_version);
	req.set_imei(s->imei);
	req.set_imsi(s->imsi);
	req.set_module_system(s->module_system);
	req.set_phone_number(s->phone_number);
	req.set_network_operator(s->network_operator);
	req.set_module_factory(s->module_factory);
	req.set_box_id(load_conf.box_id);

	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(), (int)req_len);


	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(sn);
	cmdprimitive.set_msg_id(amtpap::MsgID::MODULE_CONF_V1);
	cmdprimitive.set_direction(1);
	cmdprimitive.set_payload_data(req_buf.get(), req_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, src_id, des_id, send_buf.get(), (int)length);
	msg->body.set_src_id(load_conf.jwumq_id);
	return msg;
}


