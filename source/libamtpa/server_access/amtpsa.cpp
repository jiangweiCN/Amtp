//
//  amtpsa.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/14.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtpsa.hpp"
#include <dlfcn.h>
#include <cstring>
#include <unistd.h>
#include "../../jwumq/jwumq_message.hpp"


Amtpsa::Amtpsa()
{
	max_queue_size = 100;
	memset(mq_id, 0, sizeof(MAX_IDENTITY_BUF_SIZE));
}

Amtpsa::~Amtpsa()
{
	
}

int Amtpsa::Connection(char * mq_id, char * cmd_address, char * data_address)
{

	cmd_delear = make_unique<JwumqDealer>();
	data_delear = make_unique<JwumqDealer>();
	
	DEALER_CONF_T cmd_conf_t;
	memset(&cmd_conf_t, 0, sizeof(DEALER_CONF_T));
	memcpy(cmd_conf_t.address, cmd_address, strlen(cmd_address));
	cmd_conf_t.address[strlen(cmd_address)] = 0;
	sprintf(cmd_conf_t.identity, "%s_cmd", mq_id);
	cmd_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	cmd_conf_t.linger = 0;
	cmd_conf_t.read_timeout = 5000;
	cmd_conf_t.bind = false;
	
	int result = cmd_delear->Setup(&cmd_conf_t, std::bind(&Amtpsa::RecvCmdCallback, this, std::placeholders::_1));
	fprintf(stderr, "Command mq setup(%d)!\n", result);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}

	DEALER_CONF_T data_conf_t;
	memset(&data_conf_t, 0, sizeof(DEALER_CONF_T));
	memcpy(data_conf_t.address, data_address, strlen(data_address));
	data_conf_t.address[strlen(data_address)] = 0;
	sprintf(data_conf_t.identity, "%s_data", mq_id);
	data_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	data_conf_t.linger = 0;
	data_conf_t.read_timeout = 5000;
	data_conf_t.bind = false;
	
	result = data_delear->Setup(&data_conf_t, std::bind(&Amtpsa::RecvDataCallback, this, std::placeholders::_1));
	fprintf(stderr, "Data mq setup(%d)!\n", result);
	if(result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}
	
	memset(this->mq_id, 0, sizeof(MAX_IDENTITY_BUF_SIZE));
	memcpy(this->mq_id, mq_id, strlen(mq_id));
	
	return LIB_JWUMQ_SUCCESS;
}
void Amtpsa::Release()
{
	
}
int Amtpsa::SendCmd(uint32_t cmd, char * des_id, void * data, int data_len)
{
	
	if(data == nullptr || data_len <= 0 || des_id == nullptr || cmd_delear == nullptr || data_delear == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, "", des_id, data, data_len);

	fprintf(stderr, "Send msg, sn = %d, %d, %d, %s, %s, %d!\n", msg->body.sn(), msg->body.ts_sec(), msg->body.ts_us(), msg->body.src_id().c_str(), msg->body.des_id().c_str(), (int)msg->body.command());
	int result = 0;
	if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp))
	{
		result = data_delear->Send(msg.get());
	}
	else
	{
		result = cmd_delear->Send(msg.get());
	}

	return result;
}
int Amtpsa::ReadCmd(char * src_id, void * data, int data_len)
{
	std::unique_lock<std::mutex> lock(queue_mutex);
	JwumqMessage *msg = nullptr;
	
	if(msg_q.size() <= 0)
	{
		return LIB_AMTPA_QUEUE_EMPTY;
	}
	msg = (JwumqMessage*)msg_q.front();
	msg_q.pop();
	if(msg == nullptr)
	{
		return LIB_AMTPA_MSG_IS_NULL;
	}
	if(msg->RawDataLen() != data_len)
	{
		return LIB_AMTPA_MSG_LEN_ERROR;
	}
	
	memcpy(src_id, msg->body.src_id().c_str(), msg->body.src_id().length());
	memcpy(data, msg->RawData(), data_len);
	delete msg;
	return LIB_AMTPA_SUCCESS;
}
int Amtpsa::WaitForCmd(int * msg_len, int timeout)
{
	*msg_len = -1;
	std::unique_lock<std::mutex> lock(queue_mutex);
	if(msg_q.size() <= 0)
	{
		if(timeout <= 0)
		{
			timeout = 1000;
		}
		if(empty.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
	}
	JwumqMessage *msg = (JwumqMessage*)msg_q.front();
	if(msg == nullptr)
	{
		return LIB_AMTPA_MSG_IS_NULL;
	}
	if (msg->RawDataLen() > 0)
	{
		*msg_len = msg->RawDataLen();
		return static_cast<int>(msg_q.size());
	}
	msg_q.pop();
	if (msg != nullptr)
	{
		delete msg;
		msg = nullptr;
	}
	return LIB_AMTPA_MSG_LEN_ERROR;
}

int Amtpsa::RecvDataCallback(void * msg)
{
	if(static_cast<int>(msg_q.size()) >= max_queue_size)
	{
		return 0;
	}
	if(msg == nullptr)
	{
		return 0;
	}
	
	JwumqMessage * recv_msg = new JwumqMessage((JwumqMessage*)msg);
	
	if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data_ack, mq_id, recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));
		
		if(data_delear != nullptr)
		{
			data_delear->Send(ack_msg.get());
			fprintf(stderr, "Send msg ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		}

		std::unique_lock<std::mutex> lock(queue_mutex);
		msg_q.push(recv_msg);
		empty.notify_all();
	}
	else if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
		fprintf(stderr, "Recv msg ack, sn = %d!\n", ack_sn);
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_req))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::private_alive_resp, mq_id, recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));

		if (data_delear != nullptr)
		{
			data_delear->Send(ack_msg.get());
			fprintf(stderr, "Send data alive req ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		}
		return 0;
	}

	return 0;
}
int Amtpsa::RecvCmdCallback(void * msg)
{
	if(static_cast<int>(msg_q.size()) >= max_queue_size)
	{
		return 0;
	}
	if(msg == nullptr)
	{
		return 0;
	}
	
	JwumqMessage * recv_msg = new JwumqMessage((JwumqMessage*)msg);
	
	if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data_ack, mq_id, recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));
		
		if(cmd_delear != nullptr)
		{
			cmd_delear->Send(ack_msg.get());
			fprintf(stderr, "Send msg ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		}

		std::unique_lock<std::mutex> lock(queue_mutex);
		msg_q.push(recv_msg);
		empty.notify_all();
	}
	else if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
		fprintf(stderr, "Recv msg ack, sn = %d!\n", ack_sn);
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_req))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::private_alive_resp, "", recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));

		if (cmd_delear != nullptr)
		{
			cmd_delear->Send(ack_msg.get());
			fprintf(stderr, "Send cmd alive req ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		}
		return 0;
	}

	return 0;
}

int Amtpsa::SetLimit(int max_size)
{
	if(max_size <= 0)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	max_queue_size = max_size;
	return LIB_AMTPA_SUCCESS;
}
