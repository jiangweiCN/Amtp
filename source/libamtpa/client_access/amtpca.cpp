//
//  amtpca.cpp
//  Amtp
//
//  Created by jiangwei on 2020/2/27.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "amtpca.hpp"
//#include <dlfcn.h>
#include "../../jlog/jLog.h"
#if defined(_WIN32)
// #include <memory>
#include <windows.h>
#include "../../libjwumq/libjwumq.hpp"
#else
#include <dlfcn.h>
#endif
#include "../../amtpa_protocol/amtpap_msg.pb.h"
//#include "sys_info/linux_info.hpp"
//#include "sys_info/win_info.hpp"
#include "message/amtp_login.hpp"
#include "message/amtp_login_resp.hpp"
#include "message/amtp_logout.hpp"
#include "message/amtp_logout_resp.hpp"
#include "message/amtp_config.hpp"
#include "message/amtp_config_resp.hpp"
#include "message/amtp_config_data.hpp"
#include "message/amtp_config_data_resp.hpp"
#include "message/amtp_config_notify.hpp"
#include "message/amtp_config_notify_resp.hpp"
#include "message/amtp_upload_file.hpp"
#include "message/amtp_upload_file_resp.hpp"
#include "message/amtp_upload_eof.hpp"
#include "message/amtp_upload_eof_resp.hpp"
#include "message/amtp_query_data.hpp"
#include "message/amtp_query_data_resp.hpp"
#include "message/amtp_upload_file_data.hpp"
#include "message/amtp_upload_file_data_resp.hpp"
#include "message/amtp_module_conf.hpp"
#include "message/amtp_module_conf_resp.hpp"

Amtpca::Amtpca()
{
	lib_handle = nullptr;
#if defined(_WIN32)

#else
	jwumq_version = nullptr;
	jwumq_setup = nullptr;
	jwumq_send = nullptr;
	jwumq_release = nullptr;
#endif
	cmd_handle = nullptr;
	data_handle = nullptr;

	blog = false;
	cmd_recv_callback = nullptr;
	cmd_sn = 0;
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));

	native_event_objects = make_unique<msg_event_objects>();
	ack_objects = make_unique<msg_event_objects>();
}

Amtpca::~Amtpca()
{
}

int Amtpca::Init(const char *license_path, const char *log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back)
{
	if (log_path != nullptr)
	{
		string path = log_path;
		path.append("/libamtpca");
		JLog::Init("libamtpca", 0, path.c_str());
		blog = true;
	}
	JLOG(INFO) << "**********************libamtpca begin!************************";
	cmd_recv_callback = call_back;
	if (cmd_recv_callback == nullptr)
	{
		JLOG(INFO) << "The command callback is disable.";
	}

	if (amtp_license.Load(license_path, amtp_conf) < 0)
	{
		JLOG(ERROR) << "License load failed." << license_path;
		return LIB_AMTPA_LICENSE_ERROR;
	}
	JLOG(INFO) << "License load OK.";

#if defined(_WIN32)

#else
	lib_handle = dlopen("libjwumq.so", RTLD_NOW);
	if (lib_handle == nullptr)
	{
		JLOG(ERROR) << "Failed to open libaray 'libjwumq.so', error:" << dlerror();
		return LIB_AMTPA_OPEN_LIB_FAILED;
	}

	jwumq_version = (LIB_JWUMQ_VERSION)dlsym(lib_handle, "jwumq_version");
	jwumq_dealer_load = (LIB_JWUMQ_DEALER_LOAD)dlsym(lib_handle, "jwumq_dealer_load");
	jwumq_setup = (LIB_JWUMQ_SETUP)dlsym(lib_handle, "jwumq_setup");
	jwumq_send = (LIB_JWUMQ_SEND)dlsym(lib_handle, "jwumq_send");
	jwumq_release = (LIB_JWUMQ_RELEASE)dlsym(lib_handle, "jwumq_release");

	if (jwumq_version == nullptr)
	{
		JLOG(ERROR) << "lib_version failed.";
		return LIB_AMTPA_VERSION_FAILED;
	}

#endif

	JLOG(INFO) << "libjwumq version = " << jwumq_version();
	cmd_handle = jwumq_dealer_load();
	data_handle = jwumq_dealer_load();

#if defined(_WIN32)
	JLOG(INFO) << "libjwumq cmd_handle = " << reinterpret_cast<unsigned long long>(cmd_handle);
	JLOG(INFO) << "libjwumq data_handle = " << reinterpret_cast<unsigned long long>(data_handle);
#else
	JLOG(INFO) << "libjwumq cmd_handle = " << reinterpret_cast<unsigned long>(cmd_handle);
	JLOG(INFO) << "libjwumq data_handle = " << reinterpret_cast<unsigned long>(data_handle);
#endif

	DEALER_CONF_T cmd_conf_t;
	memset(&cmd_conf_t, 0, sizeof(DEALER_CONF_T));
#if defined(_WIN32)
	sprintf_s(cmd_conf_t.address, MAX_ADDRESS_BUF_SIZE, "tcp://%s:%d", amtp_conf.cmd_ip.c_str(), amtp_conf.cmd_port);
#else
	sprintf(cmd_conf_t.address, "tcp://%s:%d", amtp_conf.cmd_ip.c_str(), amtp_conf.cmd_port);
#endif
	cmd_mq_id = amtp_conf.jwumq_id + "_c";
	memcpy(cmd_conf_t.identity, cmd_mq_id.c_str(), cmd_mq_id.length());
	cmd_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	cmd_conf_t.linger = 0;
#if defined(_WIN32)
	cmd_conf_t.read_timeout = 1000;
#else
	cmd_conf_t.read_timeout = 3000;
#endif
	cmd_conf_t.bind = false;

	JLOG(INFO) << "libjwumq command address = " << cmd_conf_t.address << ", command identity = "
			   << cmd_conf_t.identity << ".";

	int result = jwumq_setup(cmd_handle, &cmd_conf_t, std::bind(&Amtpca::RecvCmdCallback, this, std::placeholders::_1));
	JLOG(INFO) << "Command mq lib_jwumq_setup = " << result;
	if (result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}

	DEALER_CONF_T data_conf_t;
	memset(&data_conf_t, 0, sizeof(DEALER_CONF_T));
#if defined(_WIN32)
	sprintf_s(data_conf_t.address, MAX_ADDRESS_BUF_SIZE, "tcp://%s:%d", amtp_conf.data_ip.c_str(), amtp_conf.data_port);
#else
	sprintf(data_conf_t.address, "tcp://%s:%d", amtp_conf.data_ip.c_str(), amtp_conf.data_port);
#endif
	// data_mq_id = amtp_conf.jwumq_id + "_data";
	data_mq_id = amtp_conf.jwumq_id;
	memcpy(data_conf_t.identity, data_mq_id.c_str(), data_mq_id.length());
	data_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	data_conf_t.linger = 0;
#if defined(_WIN32)
	data_conf_t.read_timeout = 1000;
#else
	data_conf_t.read_timeout = 3000;
#endif
	data_conf_t.bind = false;

	JLOG(INFO) << "libjwumq data address = " << data_conf_t.address << ", data identity = "
			   << data_conf_t.identity << ".";

	result = jwumq_setup(data_handle, &data_conf_t, std::bind(&Amtpca::RecvDataCallback, this, std::placeholders::_1));
	JLOG(INFO) << "Data mq lib_jwumq_setup = " << result;
	if (result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}
	string str = "router trace";
	unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_data, (void *)str.c_str(), (int)str.length()));
	jwumq_send(data_handle, msg.get());
	jwumq_send(cmd_handle, msg.get());

	SetupInprocMq(amtp_conf.jwumq_id.c_str());

	return LIB_JWUMQ_SUCCESS;
}

void Amtpca::SetupInprocMq(const char *mq_id)
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

	thread_loop = 1;
	data_handler_thread = thread(&Amtpca::DataHandlerThread, this);
	cmd_handler_thread = thread(&Amtpca::CmdHandlerThread, this);
}

void Amtpca::Release()
{
	thread_loop = 0;
	if (data_handler_thread.joinable())
	{
		data_handler_thread.join();
	}
	if (cmd_handler_thread.joinable())
	{
		cmd_handler_thread.join();
	}
#if defined(_WIN32)
	JLOG(INFO) << "libjwumq release cmd_handle = " << reinterpret_cast<unsigned long long>(cmd_handle);
	JLOG(INFO) << "libjwumq release data_handle = " << reinterpret_cast<unsigned long long>(data_handle);
	if (cmd_handle != nullptr)
	{
		jwumq_release(cmd_handle);
		cmd_handle = nullptr;
	}
	if (data_handle != nullptr)
	{
		jwumq_release(data_handle);
		data_handle = nullptr;
	}
#else
	if (lib_handle != nullptr)
	{
		if (cmd_handle != nullptr)
		{
			jwumq_release(cmd_handle);
			cmd_handle = nullptr;
		}
		if (data_handle != nullptr)
		{
			jwumq_release(data_handle);
			data_handle = nullptr;
		}
		dlclose(lib_handle);
	}
#endif
	JLOG(INFO) << "libamtpca end!";
	if (blog)
	{
		blog = false;
		JLog::Close();
	}
	cmd_sn = 0;
}

void Amtpca::DataHandlerThread()
{
	JLOG(INFO) << "libamtpca DataHandlerThread start!";

	int index = 0;
	while (thread_loop > 0)
	{
		struct timeval tv = {60, 0};
		select(0, NULL, NULL, NULL, &tv);
		JLOG(INFO) << "libamtpca data channel send alive packet: " << index;
		string str = "alive " + to_string(index);
		unique_ptr<JwumqMessage> data_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, data_mq_id, "", (void *)str.c_str(), (int)str.length()));
		jwumq_send(data_handle, data_msg.get());
		index++;
	}

	JLOG(INFO) << "libamtpca DataHandlerThread end!";
}

int Amtpca::RecvDataCallback(void *msg)
{
	JwumqMessage *recv_msg = new JwumqMessage((JwumqMessage *)msg);
	zmq_msg_t z_msg;
	zmq_msg_init_size(&z_msg, sizeof(recv_msg));
	memcpy(zmq_msg_data(&z_msg), &recv_msg, sizeof(recv_msg));
	zmq_msg_send(&z_msg, send_socket, ZMQ_DONTWAIT);
	zmq_msg_close(&z_msg);
	// fprintf(stderr, "+++++++++++++++++++++++++++++++RecvDataCallback sn = %d !\n", recv_msg->body.sn());
	return 0;
}

void Amtpca::CmdHandlerThread()
{
	JLOG(INFO) << "libamtpca CmdHandlerThread start!";
	int index = 0;

	zmq_pollitem_t items[] = {{recv_socket, 0, ZMQ_POLLIN, 0}};

	while (thread_loop > 0)
	{
		if (index % 20 == 0)
		{
			JLOG(INFO) << "libamtpca cmd channel send alive packet: " << index / 20;
			string str = "alive " + to_string(index / 20);
			unique_ptr<JwumqMessage> cmd_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, cmd_mq_id, "", (void *)str.c_str(), (int)str.length()));
			jwumq_send(cmd_handle, cmd_msg.get());
		}
		index++;

		zmq_poll(items, 1, 3000);
		if (items[0].revents & ZMQ_POLLIN)
		{
			zmq_msg_t recv_msg_t;
			zmq_msg_init(&recv_msg_t);
			int recv_len = zmq_msg_recv(&recv_msg_t, recv_socket, 0);
			if (recv_len <= 0)
			{
				zmq_msg_close(&recv_msg_t);
#if defined(_WIN32)
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			}

			JwumqMessage *recv_msg = nullptr;
			memcpy(&recv_msg, zmq_msg_data(&recv_msg_t), recv_len);
			zmq_msg_close(&recv_msg_t);
			if (recv_msg != nullptr)
			{
				index = 1;
				if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
				{
					uint32_t msg_sn = recv_msg->body.sn();
					unique_ptr<JwumqMessage> ack_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_ack, &msg_sn, sizeof(msg_sn)));
					if (cmd_handle != nullptr)
					{
						jwumq_send(cmd_handle, ack_msg.get());
						JLOG(INFO) << "Send Cmd ack, sn = " << msg_sn;
					}

					unique_ptr<amtpap::CmdPrimitive> resp(new amtpap::CmdPrimitive());
					resp->ParseFromArray(recv_msg->RawData(), recv_msg->RawDataLen());

					uint32_t serial_number = resp->serial_number();
					uint32_t cmd = resp->msg_id();
					JLOG(INFO) << "Recv Cmd, cmd = " << cmd << ", sn = " << serial_number;

					EVENT_OBJECT_S *obj = native_event_objects->get(serial_number);

					if (cmd == amtpap::MsgID::LOGIN_RESP_V1 && obj != nullptr)
					{
						RecvLoginResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::MODULE_CONF_RESP_V1 && obj != nullptr)
					{
						RecvModuleConfResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::CONFIG_RESP_V1 && obj != nullptr)
					{
						RecvConfigResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::CONFIG_DATA_V1)
					{
						RecvConfigData(recv_msg);
					}
					else if (cmd == amtpap::MsgID::CONFIG_NOTIFY_V1)
					{
						RecvConfigNotify(recv_msg);
					}
					else if (cmd == amtpap::MsgID::UPLOAD_FILE_RESP_V1 && obj != nullptr)
					{
						RecvUploadFileResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::UPLOAD_EOF_RESP_V1 && obj != nullptr)
					{
						RecvUploadEofResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::QUERY_DATA_V1)
					{
						RecvQueryData(recv_msg);
					}
					else if (cmd == amtpap::MsgID::LOGOUT_RESP_V1 && obj != nullptr)
					{
						RecvLogoutResp(recv_msg, obj);
					}
					else if (cmd == amtpap::MsgID::UPLOAD_FILE_DATA_RESP_V1)
					{
						RecvUploadFileDataResp(recv_msg, obj);
					}
				}
				else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
				{
					uint32_t ack_sn = 0;
					memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));

					JLOG(INFO) << "Command mq recv msg ack, sn = " << ack_sn;
					EVENT_OBJECT_S *obj = ack_objects->get(ack_sn);
					if (obj != nullptr)
					{
						obj->cv.notify_all();
					}
				}
				else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_resp))
				{
					JLOG(INFO) << "Cmd recv alive ack";
				}
				delete recv_msg;
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

	JLOG(INFO) << "libamtpca CmdHandlerThread end!";
}
int Amtpca::RecvCmdCallback(void *msg)
{
	JwumqMessage *recv_msg = new JwumqMessage((JwumqMessage *)msg);
	zmq_msg_t z_msg;
	zmq_msg_init_size(&z_msg, sizeof(recv_msg));
	memcpy(zmq_msg_data(&z_msg), &recv_msg, sizeof(recv_msg));
	zmq_msg_send(&z_msg, send_socket, ZMQ_DONTWAIT);
	zmq_msg_close(&z_msg);
	// fprintf(stderr, "+++++++++++++++++++++++++++++++RecvCmdCallback sn = %d !\n", recv_msg->body.sn());
	return 0;
}

int Amtpca::WaitForCmd(int handle, uint32_t cmd, void *msg_stru, uint32_t timeout)
{
	JLOG(INFO) << "WaitForCmd, cmd = " << cmd << ", handle = " << handle;
	int result = LIB_AMTPA_WAITFORCMD_ERROR;
	if (msg_stru == nullptr)
	{
		return result;
	}
	EVENT_OBJECT_S *obj_s = native_event_objects->add(handle, cmd);
	if (obj_s == nullptr)
	{
		fprintf(stderr, "Add EVENT_OBJECT_S failed\n");
		return result;
	}
	result = LIB_AMTPA_SUCCESS;
	unique_lock<std::mutex> lock(obj_s->mtx);
	if (obj_s->cv.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		result = LIB_AMTPA_TIMEOUT;
	}
	memcpy(msg_stru, obj_s->msg_stru_data, obj_s->msg_stru_data_len);
	native_event_objects->remove(handle);
	return result;
}

int Amtpca::SendCmd(uint32_t cmd, void *cmd_s, bool sync, uint32_t timeout)
{
#if defined(_WIN32)
#else
	if (jwumq_send == nullptr)
	{
		return LIB_AMTPA_NO_INIT;
	}
#endif
	switch (cmd)
	{
	case static_cast<uint32_t>(AMTP_CMD_ENUM::login):
	{
		return SendLogin((LOGIN_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf):
	{
		return SendModuleConf((MODULE_CONF_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::config):
	{
		return SendConfig((CONFIG_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::config_data_resp):
	{
		return SendConfigDataResp((CONFIG_DATA_RESP_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::config_notify_resp):
	{
		return SendConfigNotifyResp((CONFIG_NOTIFY_RESP_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::query_data_resp):
	{
		return SendQueryDataResp((QUERY_DATA_RESP_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file):
	{
		return SendUploadFile((UPLOAD_FILE_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data):
	{
		return SendUploadFileData((UPLOAD_FILE_DATA_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof):
	{
		return SendUploadEof((UPLOAD_EOF_STRU *)cmd_s, sync, timeout);
		break;
	}
	case static_cast<uint32_t>(AMTP_CMD_ENUM::logout):
	{
		return SendLogout(sync, timeout);
		break;
	}

	default:
		break;
	}
	return LIB_AMTPA_SEND_ERROR;
}

int Amtpca::SendLogin(LOGIN_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_login login;
	int handle = cmd_sn;
	msg = login.data((LOGIN_STRU *)s, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}

	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send login msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send login msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send login msg, get ack " << msg->body.sn();

	return handle;
}
void Amtpca::RecvLoginResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_login_resp resp;
	LOGIN_RESP_STRU login_resp_tmp;
	resp.data(recv_msg, login_resp_tmp);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(LOGIN_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(LOGIN_RESP_STRU);
		memcpy(obj->msg_stru_data, &login_resp_tmp, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::login_resp, &login_resp_tmp, nullptr);
	}
	JLOG(INFO) << "Recv login response, manufacturer = " << login_resp_tmp.manufacturer
			   << ", token = " << token
			   << ", result = " << login_resp_tmp.result;
}
int Amtpca::SendLogout(bool sync, uint32_t timeout)
{
	if (cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_logout logout;
	int handle = cmd_sn;
	msg = logout.data(token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send logout msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send logout msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send logout msg, get ack " << msg->body.sn();
	return handle;
}
void Amtpca::RecvLogoutResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_logout_resp resp;
	LOGOUT_RESP_STRU logout_resp_stru;
	resp.data(recv_msg, logout_resp_stru);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(LOGOUT_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(LOGOUT_RESP_STRU);
		memcpy(obj->msg_stru_data, &logout_resp_stru, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::logout_resp, &logout_resp_stru, nullptr);
	}
	JLOG(INFO) << "Recv logout response "
			   << ", token = " << token
			   << ", result = " << logout_resp_stru.result;
}
int Amtpca::SendModuleConf(MODULE_CONF_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_module_conf conf;
	int handle = cmd_sn;
	msg = conf.data(s, token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send module config msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send module config msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send module config msg, get ack " << msg->body.sn();
	return handle;
}
void Amtpca::RecvModuleConfResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_module_conf_resp resp;
	MODULE_CONF_RESP_STRU module_conf_resp_stru;
	resp.data(recv_msg, module_conf_resp_stru);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(MODULE_CONF_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(MODULE_CONF_RESP_STRU);
		memcpy(obj->msg_stru_data, &module_conf_resp_stru, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::module_conf_resp, &module_conf_resp_stru, nullptr);
	}
	JLOG(INFO) << "Recv module conf response "
			   << ", token = " << token
			   << ", result = " << module_conf_resp_stru.result;
}
int Amtpca::SendConfig(CONFIG_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config config;
	int handle = cmd_sn;
	msg = config.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send config conig msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send config msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config msg, get ack " << msg->body.sn();
	return handle;
}
void Amtpca::RecvConfigResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_config_resp resp;
	CONFIG_RESP_STRU config_resp_stru;
	resp.data(recv_msg, config_resp_stru);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(CONFIG_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(CONFIG_RESP_STRU);
		memcpy(obj->msg_stru_data, &config_resp_stru, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_resp, &config_resp_stru, nullptr);
	}
	JLOG(INFO) << "Recv config response "
			   << ", token = " << token
			   << ", atuid_version = " << config_resp_stru.atuid_version
			   << ", md5 = " << config_resp_stru.md5
			   << ", result = " << config_resp_stru.result;
}
void Amtpca::RecvConfigData(JwumqMessage *recv_msg)
{
	amtp_config_data req;
	CONFIG_DATA_STRU config_data_tmp_s;
	memset(&config_data_tmp_s, 0, sizeof(CONFIG_DATA_STRU));
	bool result = req.data(recv_msg, &config_data_tmp_s);
	if (!result)
	{
		JLOG(INFO) << "Recv config data request, but data format error ";
		return;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);

	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_data, &config_data_tmp_s, nullptr);
	}
	if (config_data_tmp_s.config_data != nullptr)
	{
		delete[] config_data_tmp_s.config_data;
	}

	JLOG(INFO) << "Recv config data request "
			   << ", token = " << token
			   << ", atuid_version = " << config_data_tmp_s.atuid_version
			   << ", packet_count = " << config_data_tmp_s.packet_count
			   << ", packet_no = " << config_data_tmp_s.packet_no
			   << ", data_length = " << config_data_tmp_s.data_length;
}
int Amtpca::SendConfigDataResp(CONFIG_DATA_RESP_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config_data_resp config_data_resp;
	msg = config_data_resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (!sync)
	{
		return result;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send config config data resp msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send config config data resp msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config config data resp msg, get ack " << msg->body.sn();
	return result;
}
void Amtpca::RecvConfigNotify(JwumqMessage *recv_msg)
{
	amtp_config_notify req;
	CONFIG_NOTIFY_STRU config_notify_tmp_s;
	memset(&config_notify_tmp_s, 0, sizeof(CONFIG_NOTIFY_STRU));
	bool result = req.data(recv_msg, &config_notify_tmp_s);
	if (!result)
	{
		JLOG(INFO) << "Recv config notify request, but data format error ";
		return;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);

	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_notify, &config_notify_tmp_s, nullptr);
	}

	JLOG(INFO) << "Recv config notify request "
			   << ", token = " << token
			   << ", atuid_version = " << config_notify_tmp_s.atuid_version
			   << ", packet_count = " << config_notify_tmp_s.packet_count;
}
int Amtpca::SendConfigNotifyResp(CONFIG_NOTIFY_RESP_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config_notify_resp config_notify_resp;
	msg = config_notify_resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (!sync)
	{
		return result;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send config nofity resp msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send config nofity resp msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config nofity resp msg, get ack " << msg->body.sn();
	return result;
}
int Amtpca::SendUploadFile(UPLOAD_FILE_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_file upload_file;
	int handle = cmd_sn;
	msg = upload_file.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send upload file msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send upload file msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload file msg, get ack " << msg->body.sn();
	return handle;
}
void Amtpca::RecvUploadFileResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_upload_file_resp resp;
	UPLOAD_FILE_RESP_STRU upload_file_resp;

	resp.data(recv_msg, upload_file_resp);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(UPLOAD_FILE_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(UPLOAD_FILE_RESP_STRU);
		memcpy(obj->msg_stru_data, &upload_file_resp, obj->msg_stru_data_len);
		obj->cv.notify_all();
		JLOG(INFO) << "notify_all upload file msg, sn " << obj->handle;
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_file_resp, &upload_file_resp, nullptr);
	}
	JLOG(INFO) << "Recv upload file response "
			   << ", token = " << token
			   << ", file_name = " << upload_file_resp.file_name
			   << ", file_id = " << upload_file_resp.file_id
			   << ", file_size = " << upload_file_resp.file_size
			   << ", module = " << upload_file_resp.module
			   << ", result = " << upload_file_resp.result;
}
int Amtpca::SendUploadEof(UPLOAD_EOF_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_eof upload_eof;
	int handle = cmd_sn;
	msg = upload_eof.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send upload eof msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send upload eof msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload eof msg, get ack " << msg->body.sn();
	return handle;
}
void Amtpca::RecvUploadEofResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_upload_eof_resp resp;
	UPLOAD_EOF_RESP_STRU upload_eof_resp_stru;
	resp.data(recv_msg, upload_eof_resp_stru);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(UPLOAD_EOF_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(UPLOAD_EOF_RESP_STRU);
		memcpy(obj->msg_stru_data, &upload_eof_resp_stru, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}
	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_eof_resp, &upload_eof_resp_stru, nullptr);
	}
	JLOG(INFO) << "Recv upload eof response "
			   << ", token = " << token
			   << ", file_name = " << upload_eof_resp_stru.file_name
			   << ", file_id = " << upload_eof_resp_stru.file_id
			   << ", packet_count = " << upload_eof_resp_stru.packet_count
			   << ", module = " << upload_eof_resp_stru.module
			   << ", result = " << upload_eof_resp_stru.result;
}
void Amtpca::RecvQueryData(JwumqMessage *recv_msg)
{
	amtp_query_data req;
	QUERY_DATA_STRU query_data_tmp_s;
	memset(&query_data_tmp_s, 0, sizeof(QUERY_DATA_STRU));
	bool result = req.data(recv_msg, &query_data_tmp_s);
	if (!result)
	{
		JLOG(INFO) << "Recv query data request, but data format error ";
		return;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);

	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::query_data, &query_data_tmp_s, nullptr);
	}

	JLOG(INFO) << "Recv query data "
			   << ", token = " << token
			   << ", file_name = " << query_data_tmp_s.file_name
			   << ", file_id = " << query_data_tmp_s.file_id
			   << ", packet_no = " << query_data_tmp_s.packet_no;
}
int Amtpca::SendQueryDataResp(QUERY_DATA_RESP_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_query_data_resp resp;
	msg = resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(cmd_handle, msg.get());
	if (!sync)
	{
		return result;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send query data resp msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send query data resp msg, ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send query data resp msg, get ack " << msg->body.sn();
	return result;
}
int Amtpca::SendUploadFileData(UPLOAD_FILE_DATA_STRU *s, bool sync, uint32_t timeout)
{
	if (s == nullptr || data_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}

	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_file_data upload_file_data;
	int handle = cmd_sn;
	msg = upload_file_data.data(s, token, amtp_conf, cmd_sn++, data_mq_id, "");
	if (msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	int result = jwumq_send(data_handle, msg.get());
	if (result != 0)
	{
		return result;
	}
	if (!sync)
	{
		return handle;
	}
	uint32_t sn = msg->body.sn();
	JLOG(INFO) << "Jwumq send upload file data msg, sn = " << sn;
	if (WaitForAck(sn, timeout) < 0)
	{
		JLOG(INFO) << "Jwumq send upload file data msg, ack_cv.wait_for timeout, sn = " << sn;
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload file data msg, get ack " << sn;
	return handle;
}
void Amtpca::RecvUploadFileDataResp(JwumqMessage *recv_msg, EVENT_OBJECT_S *obj)
{
	amtp_upload_file_data_resp resp;
	UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_stru;
	resp.data(recv_msg, upload_file_data_resp_stru);
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	if (obj != nullptr)
	{
		if (obj->msg_stru_data != nullptr)
		{
			delete[] obj->msg_stru_data;
		}
		obj->msg_stru_data = new uint8_t[sizeof(UPLOAD_FILE_DATA_RESP_STRU)];
		obj->msg_stru_data_len = sizeof(UPLOAD_FILE_DATA_RESP_STRU);
		memcpy(obj->msg_stru_data, &upload_file_data_resp_stru, obj->msg_stru_data_len);
		obj->cv.notify_all();
	}

	if (cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_file_data_resp, &upload_file_data_resp_stru, nullptr);
	}
	JLOG(INFO) << "Recv upload file data response "
			   << ", token = " << token
			   << ", file_id = " << upload_file_data_resp_stru.file_id
			   << ", packet_no = " << upload_file_data_resp_stru.packet_no
			   << ", result = " << upload_file_data_resp_stru.result;
}
int Amtpca::WaitForAck(uint32_t handle, int timeout)
{
	int result = 1;
	// EVENT_OBJECT_S *ack_obj = ack_objects->add(handle, static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack));
	// if (ack_obj == nullptr)
	// {
	// 	return 0;
	// }
	// unique_lock<std::mutex> lock(ack_obj->mtx);
	// if (ack_obj->cv.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	// {
	// 	result = -1;
	// }
	// ack_objects->remove(handle);
	return result;
}
