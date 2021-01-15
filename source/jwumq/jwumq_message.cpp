//
//  jwumq_message.cpp
//  Amtp
//
//  Created by jiangwei on 2020/7/24.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "jwumq_message.hpp"
#if defined(_WIN32)
#include <time.h>
#include <Windows.h> 
#else
#include <sys/time.h>
#endif

//#include "../jlog/jLog.h"

static uint32_t sn = 0;
JwumqMessage::JwumqMessage(const void * msg_data, int msg_data_len)
{
	body.ParseFromArray(msg_data, msg_data_len);
	this->msg_data_len = (uint32_t)msg_data_len;
	this->msg_data = make_unique<unsigned char[]>(this->msg_data_len);
	memcpy(this->msg_data.get(), msg_data, msg_data_len);
}
JwumqMessage::JwumqMessage(JWUMQ_COMMAND_ENUM command, const void * raw_data, int raw_data_len)
{
	new (this)JwumqMessage(command, "", "", "", raw_data, raw_data_len);
}

JwumqMessage::JwumqMessage(JWUMQ_COMMAND_ENUM command, string src_id, string des_id, const void * raw_data, int raw_data_len)
{
	new (this)JwumqMessage(command, src_id, des_id, "", raw_data, raw_data_len);
}

JwumqMessage::JwumqMessage(JWUMQ_COMMAND_ENUM command, string src_id, string des_id, string queue_name, const void * raw_data, int raw_data_len)
{
#if defined(_WIN32)
	time_t now_sec;
	now_sec = time(NULL);
	this->body.set_ts_sec((uint32_t)now_sec);

	uint32_t now_ms;
	now_ms = GetTickCount();
	this->body.set_ts_us(now_ms * 1000);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	this->body.set_ts_sec((uint32_t)tv.tv_sec);
	this->body.set_ts_us(tv.tv_usec);
#endif
	this->body.set_command(static_cast<uint32_t>(command));
	this->body.set_sn(sn++);
	this->body.set_src_id(src_id);
	this->body.set_des_id(des_id);
	this->body.set_queue_name(queue_name);
	this->body.set_raw_data_len(raw_data_len);
	this->body.set_raw_data(raw_data, raw_data_len);
	
	this->msg_data_len = (uint32_t)this->body.ByteSizeLong();
	this->msg_data = make_unique<unsigned char[]>(this->msg_data_len);
	this->body.SerializeToArray(this->msg_data.get(), this->msg_data_len);
}
JwumqMessage::JwumqMessage(JwumqMessage * jmsg)
{
	this->body = jmsg->body;
	this->msg_data_len = (uint32_t)this->body.ByteSizeLong();
	this->msg_data = make_unique<unsigned char[]>(this->msg_data_len);
	this->body.SerializeToArray(this->msg_data.get(), this->msg_data_len);
}

JwumqMessage::JwumqMessage()
{

}
JwumqMessage::~JwumqMessage()
{
}

int JwumqMessage::MsgDataLen()
{
	return msg_data_len;
}
//
void* JwumqMessage::MsgData()
{
	return (void*)this->msg_data.get();
}

int JwumqMessage::RawDataLen()
{
	return this->body.raw_data_len();
}

void* JwumqMessage::RawData()
{
	return (void*)this->body.raw_data().c_str();
}
void JwumqMessage::SetDestination(string des_id)
{
	this->msg_data.reset();
	this->body.set_des_id(des_id);
	this->msg_data_len = (uint32_t)this->body.ByteSizeLong();
	this->msg_data = make_unique<unsigned char[]>(this->msg_data_len);
	this->body.SerializeToArray(this->msg_data.get(), this->msg_data_len);
}

string JwumqMessage::Print()
{
	string text = "";
	text += "command:" + to_string(this->body.command()) + ", ";
	text += "ts_sec:" + to_string(this->body.ts_sec()) + ", ";
	text += "ts_us:" + to_string(this->body.ts_us()) + ", ";
	text += "sn:" + to_string(this->body.sn()) + ", ";
	text += "src_id:" + this->body.src_id() + ", ";
	text += "des_id:" + this->body.des_id() + ", ";
	text += "queue_name:" + this->body.queue_name() + ", ";
	text += "raw_data_len:" + to_string(this->body.raw_data_len()) + ", ";
	
	text += "raw_data:";
	unsigned char * data = (unsigned char*)this->body.raw_data().c_str();
	for(int x = 0; x < (int)this->body.raw_data_len(); x++)
	{
		char c[8];
#if defined(_WIN32)
		sprintf_s(c, 8, "%02X, ", data[x]);  // 16进制
#else
		sprintf(c, "%02X, ", data[x]);  // 16进制
#endif
		
		text += c;
	}
	
	return text;
}
