//
//  amtp_gps_info.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_gps_info.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"

amtp_gps_info::amtp_gps_info()
{
	
}

amtp_gps_info::~amtp_gps_info()
{
	
}

unique_ptr<JwumqMessage> amtp_gps_info::data(GPS_INFO_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::GPSInfoV1 req;
	req.set_token((char*)token);
	req.set_longitude(s->longitude);
	req.set_latitude(s->latitude);
	req.set_altitude(s->altitude);
	req.set_speed(s->speed);
	req.set_heading(s->heading);
	req.set_ts_sec(s->ts_sec);
	req.set_ts_usec(s->ts_usec);
	
	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(), (int)req_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(sn);
	cmdprimitive.set_msg_id(amtpap::MsgID::GPS_INFO_V1);
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

