//
//  amtp_upload_file_data.cpp
//  Amtp
//
//  Created by jiangwei on 2020/3/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtp_upload_file_data.hpp"
#include "../../../amtpa_protocol/amtpap_msg.pb.h"
#include <zlib.h>

amtp_upload_file_data::amtp_upload_file_data()
{
	
}

amtp_upload_file_data::~amtp_upload_file_data()
{
	
}

unique_ptr<JwumqMessage> amtp_upload_file_data::data(UPLOAD_FILE_DATA_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id)
{
	amtpap::UploadFileDataV1 req;
	req.set_token((char*)token);
	req.set_file_id(s->file_id);
	req.set_packet_no(s->packet_no);
	req.set_piece_len(s->data_len);
	
	unique_ptr<unsigned char[]> com_data(new unsigned char[s->data_len]());
	int err;
	unsigned long com_data_len = s->data_len;
	err = compress(com_data.get(), &com_data_len, s->data, s->data_len);
	if (err != Z_OK)
	{
//		cout << "compess error: " << err << endl;
		fprintf(stderr, "compess error: %d\n", err);
	}
//	cout << "orignal size: " << len << " , compressed size : " << comprLen << endl;
//	req.set_data_len(s->data_len);
//	req.set_data(s->data, s->data_len);
	req.set_data_len(com_data_len);
	req.set_data(com_data.get(), com_data_len);
	
	size_t req_len = req.ByteSizeLong();
	unique_ptr<char[]> req_buf(new char[req_len]());
	req.SerializeToArray(req_buf.get(), (int)req_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(sn);
	cmdprimitive.set_msg_id(amtpap::MsgID::UPLOAD_FILE_DATA_V1);
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

