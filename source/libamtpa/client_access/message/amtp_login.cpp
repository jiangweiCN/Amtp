//
//  amtp_login.cpp
//  Amtp
//
//  Created by jiangwei on 2020/2/29.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_login.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"


amtp_login::amtp_login()
{
	
}

amtp_login::~amtp_login()
{
	
}

unique_ptr<JwumqMessage> amtp_login::data(LOGIN_STRU * login_s, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::LoginV1 login;
//	login.set_cmd_ip(license_info.cmd_ip());
//	login.set_cmd_port(license_info.cmd_port());
	login.set_box_id(load_conf.box_id);
//	login.set_pwd(load_conf.pwd);
	login.set_pwd(login_s->manufacturer_pwd);
	login.set_manufactor(load_conf.manufactor);
	login.set_begin_time(load_conf.begin_time);
	login.set_end_time(load_conf.end_time);
	login.set_release_id(load_conf.release_id);
	login.set_jwumq_id(load_conf.jwumq_id);
//	login.set_conf_version(login_s->conf_version);

	size_t login_len = login.ByteSizeLong();
	unique_ptr<char[]> login_buf(new char[login_len]());
	login.SerializeToArray(login_buf.get(), (int)login_len);


	amtpap::CmdPrimitive req;
	req.set_type(amtpap::MsgType::PUBLIC);
	req.set_serial_number(sn);
	req.set_msg_id(amtpap::MsgID::LOGIN_V1);
	req.set_direction(1);
	req.set_payload_data(login_buf.get(), login_len);

	size_t length = req.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	req.SerializeToArray(send_buf.get(), (int)length);

	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, src_id, des_id, send_buf.get(), (int)length);
//	strcpy(msg->body.src_id, license_info.jwumq_id().c_str());
	msg->body.set_src_id(load_conf.jwumq_id);
	
	return msg;
}

