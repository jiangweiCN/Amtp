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
#include "../../jwumq_external/jwumq_message.hpp"
#include <chrono>

Amtpsa::Amtpsa()
{
	// max_queue_size = 100;
	msg_for_read = nullptr;
	memset(mq_id, 0, sizeof(MAX_IDENTITY_BUF_SIZE));
}

Amtpsa::~Amtpsa()
{
	Release();
}

int Amtpsa::Connection(char *mq_id, char *cmd_address, char *data_address)
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
	cmd_conf_t.read_timeout = 3000;
	cmd_conf_t.bind = false;

	int result = cmd_delear->Setup(&cmd_conf_t, std::bind(&Amtpsa::RecvCmdCallback, this, std::placeholders::_1));
	fprintf(stderr, "Command mq setup(%d)!\n", result);
	if (result != LIB_JWUMQ_SUCCESS)
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
	data_conf_t.read_timeout = 3000;
	data_conf_t.bind = false;

	result = data_delear->Setup(&data_conf_t, std::bind(&Amtpsa::RecvDataCallback, this, std::placeholders::_1));
	fprintf(stderr, "Data mq setup(%d)!\n", result);
	if (result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}

	memset(this->mq_id, 0, MAX_IDENTITY_BUF_SIZE);
	memcpy(this->mq_id, mq_id, strlen(mq_id));

	SetupInprocMq(mq_id);
	return LIB_JWUMQ_SUCCESS;
}

void Amtpsa::SetupInprocMq(char *mq_id)
{
	sprintf(inproc_send_id, "%s_inproc_send", mq_id);
	sprintf(inproc_recv_id, "%s_inproc_recv", mq_id);

	inproc_context = zmq_ctx_new();

	send_socket = zmq_socket(inproc_context, ZMQ_DEALER);
	recv_socket = zmq_socket(inproc_context, ZMQ_DEALER);
	zmq_setsockopt(send_socket, ZMQ_IDENTITY, inproc_send_id, strlen(inproc_send_id));
	zmq_setsockopt(recv_socket, ZMQ_IDENTITY, inproc_recv_id, strlen(inproc_recv_id));
#if defined(_WIN32)
	int timeout = 1000;
	zmq_setsockopt(send_socket, ZMQ_SNDTIMEO, &timeout, timeout);
	zmq_setsockopt(recv_socket, ZMQ_SNDTIMEO, &timeout, timeout);
#else
#endif
	memset(inproc_address, 0, MAX_ADDRESS_BUF_SIZE);
	sprintf(inproc_address, "inproc://%s", mq_id);
	zmq_bind(recv_socket, inproc_address);
	zmq_connect(send_socket, inproc_address);

	// thread_loop = 1;
	// msg_handler_thread = thread(&Amtpsa::MsgHandlerThread, this);
}

void Amtpsa::Release()
{
	// thread_loop = 0;
	// if (msg_handler_thread.joinable())
	// {
	// 	msg_handler_thread.join();
	// }
}
int Amtpsa::SendCmd(uint32_t cmd, char *des_id, void *data, int data_len)
{

	if (data == nullptr || data_len <= 0 || des_id == nullptr || cmd_delear == nullptr || data_delear == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	int result = 0;
	if (cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp))
	{
		// char msg_des_id[MAX_IDENTITY_BUF_SIZE] = {0};
		string msg_des_id = des_id;
		msg_des_id.append("_c");
		unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, "", msg_des_id.c_str(), data, data_len);
		// result = data_delear->Send(msg.get());
		result = cmd_delear->Send(msg.get());
	}
	else
	{
		unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, "", des_id, data, data_len);
		result = cmd_delear->Send(msg.get());
	}

	return result;
}

int Amtpsa::ReadCmd(char *src_id, void *data, int data_len)
{
	if (msg_for_read == nullptr)
	{
		return LIB_AMTPA_MSG_IS_NULL;
	}
	if (msg_for_read->RawDataLen() != data_len)
	{
		delete msg_for_read;
		msg_for_read = nullptr;
		return LIB_AMTPA_MSG_LEN_ERROR;
	}
	memcpy(src_id, msg_for_read->body.src_id().c_str(), msg_for_read->body.src_id().length());
	memcpy(data, msg_for_read->RawData(), data_len);
	delete msg_for_read;
	msg_for_read = nullptr;
	return LIB_AMTPA_SUCCESS;
}
int Amtpsa::WaitForCmd(int *msg_len, int timeout)
{
	*msg_len = -1;

	zmq_pollitem_t items[] = {{recv_socket, 0, ZMQ_POLLIN, 0}};
	while (1)
	{
		zmq_poll(items, 1, timeout);
		if (items[0].revents & ZMQ_POLLIN)
		{
			zmq_msg_t recv_msg;
			zmq_msg_init(&recv_msg);
			int recv_len = zmq_msg_recv(&recv_msg, recv_socket, 0);
			if (recv_len <= 0)
			{
				zmq_msg_close(&recv_msg);
#if defined(_WIN32)
				Sleep(1);
#else
				usleep(1000);
#endif
				break;
			}
			if (msg_for_read != nullptr)
			{
				delete msg_for_read;
				msg_for_read = nullptr;
			}
			memcpy(&msg_for_read, zmq_msg_data(&recv_msg), recv_len);
			zmq_msg_close(&recv_msg);
			if (msg_for_read != nullptr)
			{
				*msg_len = msg_for_read->RawDataLen();
				// fprintf(stderr, "+++++++++++++++++++++++++++++++WaitForCmd sn = %d!\n", msg_for_read->body.sn());
				return 1;
			}
		}
		else
		{
#if defined(_WIN32)
			Sleep(1);
#else
			usleep(1000);
#endif
			continue;
		}
	}
	return LIB_AMTPA_TIMEOUT;
}

int Amtpsa::RecvDataCallback(void *msg)
{
	if (msg == nullptr)
	{
		return 0;
	}

	JwumqMessage *recv_msg = new JwumqMessage((JwumqMessage *)msg);

	if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		zmq_msg_t z_msg;
		zmq_msg_init_size(&z_msg, sizeof(recv_msg));
		memcpy(zmq_msg_data(&z_msg), &recv_msg, sizeof(recv_msg));
		zmq_msg_send(&z_msg, send_socket, ZMQ_DONTWAIT);
		zmq_msg_close(&z_msg);
		// fprintf(stderr, "---------------Callback unlock Recv msg ack, sn = %d, src_id = %s, ptr = %lu, %lu!\n", msg_sn, recv_msg->body.src_id().c_str(), &recv_msg, &msg);
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
		fprintf(stderr, "Recv msg ack, sn = %d!\n", ack_sn);
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_req))
	{
		fprintf(stderr, "Recv data alive req ack, src_id = %s!\n", recv_msg->body.src_id().c_str());
		// uint32_t msg_sn = recv_msg->body.sn();
		// unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::private_alive_resp, mq_id, recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));

		// if (data_delear != nullptr)
		// {
		// 	data_delear->Send(ack_msg.get());
		// 	fprintf(stderr, "Send data alive req ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		// }
		return 0;
	}
	return 0;
}
int Amtpsa::RecvCmdCallback(void *msg)
{
	if (msg == nullptr)
	{
		return 0;
	}

	JwumqMessage *recv_msg = new JwumqMessage((JwumqMessage *)msg);

	if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		// uint32_t msg_sn = recv_msg->body.sn();
		// unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data_ack, mq_id, recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));

		// if (cmd_delear != nullptr)
		// {
		// 	cmd_delear->Send(ack_msg.get());
		// 	// fprintf(stderr, "Send msg ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		// }

		zmq_msg_t msg;
		zmq_msg_init_size(&msg, sizeof(recv_msg));
		memcpy(zmq_msg_data(&msg), &recv_msg, sizeof(recv_msg));
		zmq_msg_send(&msg, send_socket, ZMQ_DONTWAIT);
		zmq_msg_close(&msg);
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_req))
	{
		fprintf(stderr, "Recv cmd alive req ack, src_id = %s!\n", recv_msg->body.src_id().c_str());
		// uint32_t msg_sn = recv_msg->body.sn();
		// unique_ptr<JwumqMessage> ack_msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::private_alive_resp, "", recv_msg->body.src_id(), &msg_sn, sizeof(msg_sn));

		// if (cmd_delear != nullptr)
		// {
		// 	cmd_delear->Send(ack_msg.get());
		// 	fprintf(stderr, "Send cmd alive req ack, sn = %d, src_id = %s!\n", msg_sn, recv_msg->body.src_id().c_str());
		// }
		return 0;
	}
	return 0;
}

int Amtpsa::SetLimit(int max_size)
{
	// if(max_size <= 0)
	// {
	// 	return LIB_AMTPA_PARA_ERROR;
	// }
	// max_queue_size = max_size;
	return LIB_AMTPA_SUCCESS;
}
