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
#include <memory>
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
#include "message/amtp_event.hpp"
#include "message/amtp_event_resp.hpp"
#include "message/amtp_alarm.hpp"
#include "message/amtp_alarm_resp.hpp"
#include "message/amtp_alarm_clear.hpp"
#include "message/amtp_alarm_clear_resp.hpp"
#include "message/amtp_config.hpp"
#include "message/amtp_config_resp.hpp"
#include "message/amtp_gps_info.hpp"
#include "message/amtp_gps_info_resp.hpp"
#include "message/amtp_status_info.hpp"
#include "message/amtp_status_info_resp.hpp"
#include "message/amtp_config_data.hpp"
#include "message/amtp_config_data_resp.hpp"
#include "message/amtp_config_notify.hpp"
#include "message/amtp_config_notify_resp.hpp"
#include "message/amtp_restart_cmd.hpp"
#include "message/amtp_restart_cmd_resp.hpp"
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
	data_sn = 0;
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	
	recv_mutex_map[AMTP_CMD_ENUM::login_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::login_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::logout_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::logout_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::report_event_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::report_event_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::alarm_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::alarm_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::config_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::config_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::alarm_clear_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::alarm_clear_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::gps_info_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::gps_info_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::status_info_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::status_info_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::upload_file_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::upload_file_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::upload_eof_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::upload_eof_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::upload_file_data_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::upload_file_data_resp] = make_unique<condition_variable>();
	recv_mutex_map[AMTP_CMD_ENUM::module_conf_resp] = make_unique<mutex>();
	recv_cv_map[AMTP_CMD_ENUM::module_conf_resp] = make_unique<condition_variable>();
}
 
Amtpca::~Amtpca()
{
	
}

int Amtpca::Init(const char * license_path, const char * log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back)
{
	if(log_path != nullptr)
	{
		string path = log_path;
		path.append("/libamtpca");
		JLog::Init("libamtpca", 0, path.c_str());
		blog = true;
	}
	JLOG(INFO) << "**********************libamtpca begin!************************";
	cmd_recv_callback = call_back;
	if(cmd_recv_callback == nullptr)
	{
//		cmd_recv_callback(AMTP_CMD_ENUM::reserve, nullptr, nullptr);
		JLOG(INFO) << "The command callback is disable.";
	}
	
	if(amtp_license.Load(license_path, amtp_conf) < 0)
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
	cmd_mq_id = amtp_conf.jwumq_id + "_cmd";
	memcpy(cmd_conf_t.identity, cmd_mq_id.c_str(), cmd_mq_id.length());
	cmd_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	cmd_conf_t.linger = 0;
#if defined(_WIN32)
	cmd_conf_t.read_timeout = 100;
#else
	cmd_conf_t.read_timeout = 5000;
#endif
	cmd_conf_t.bind = false;

	JLOG(INFO) << "libjwumq command address = " << cmd_conf_t.address << ", command identity = "
		<< cmd_conf_t.identity << ".";

	int result = jwumq_setup(cmd_handle, &cmd_conf_t, std::bind(&Amtpca::RecvCmdCallback, this
		, std::placeholders::_1));
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
	data_mq_id = amtp_conf.jwumq_id + "_data";
	memcpy(data_conf_t.identity, data_mq_id.c_str(), data_mq_id.length());
	data_conf_t.type = JWUMQ_TYPE_ENUM::dealer;
	data_conf_t.linger = 0;
#if defined(_WIN32)
	data_conf_t.read_timeout = 100;
#else
	data_conf_t.read_timeout = 5000;
#endif
	data_conf_t.bind = false;

	JLOG(INFO) << "libjwumq data address = " << data_conf_t.address << ", data identity = "
		<< data_conf_t.identity << ".";

	result = jwumq_setup(data_handle, &data_conf_t, std::bind(&Amtpca::RecvDataCallback, this
		, std::placeholders::_1));
	JLOG(INFO) << "Data mq lib_jwumq_setup = " << result;
	if (result != LIB_JWUMQ_SUCCESS)
	{
		Release();
		return result;
	}
	string str = "router trace";
	unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_data, (void*)str.c_str(), (int)str.length()));
	jwumq_send(data_handle, msg.get());
	jwumq_send(cmd_handle, msg.get());

	alive_thread_loop = 1;
	alive_thread = thread(&Amtpca::AliveThread, this);

	return LIB_JWUMQ_SUCCESS;
}
void Amtpca::Release()
{
	alive_thread_loop = 0;
	if (alive_thread.joinable())
	{
		alive_thread.join();
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
	if(blog)
	{
		blog = false;
		JLog::Close();
	}
	cmd_sn = 0;
}

void Amtpca::AliveThread()
{
	JLOG(INFO) << "libamtpca AliveThread start!";

	int index = 0;
	while (alive_thread_loop > 0)
	{
		if (index % 600 == 0)
		{
			JLOG(INFO) << "libamtpca send alive packet: " << index/600;
			string str = "alive " + to_string(index / 600);
			unique_ptr<JwumqMessage> cmd_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, cmd_mq_id, "", (void *)str.c_str(), (int)str.length()));
			jwumq_send(cmd_handle, cmd_msg.get());

			unique_ptr<JwumqMessage> data_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::private_alive_req, data_mq_id, "", (void *)str.c_str(), (int)str.length()));
			jwumq_send(data_handle, data_msg.get());
		}
		index++;
#if defined(_WIN32)
		Sleep(100);
#else
		usleep(100 * 1000);
#endif
	}
	
	JLOG(INFO) << "libamtpca AliveThread end!";
}

int Amtpca::RecvDataCallback(void * msg)
{
	JwumqMessage * recv_msg = new JwumqMessage((JwumqMessage *)msg);
	
	if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_ack, &msg_sn, sizeof(msg_sn)));
		if(data_handle != nullptr)
		{
			jwumq_send(data_handle, ack_msg.get());
			JLOG(INFO) << "Send Cmd ack, sn = " << msg_sn;
		}
			
		amtpap::CmdPrimitive resp;
		resp.ParseFromArray(recv_msg->RawData(),recv_msg->RawDataLen());

		JLOG(INFO) << "Recv Cmd, sn = " << resp.serial_number();
			
		if(resp.msg_id() == amtpap::MsgID::UPLOAD_FILE_DATA_RESP_V1)
		{
			RecvUploadFileDataResp(recv_msg);
			if (recv_mutex_map.find(AMTP_CMD_ENUM::upload_file_data_resp) != recv_mutex_map.end()
					&& recv_cv_map.find(AMTP_CMD_ENUM::upload_file_data_resp) != recv_cv_map.end())
			{
				std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::upload_file_data_resp]);
				recv_cv_map[AMTP_CMD_ENUM::upload_file_data_resp]->notify_all();
			}
		}
		else
		{
				
		}
	}
	else if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		std::unique_lock<std::mutex> ack_lock(ack_mutex);
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
			ack_cv.notify_all();
			
		JLOG(INFO) << "Data mq recv msg ack, sn = " << ack_sn;
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_resp))
	{
		JLOG(INFO) << "Data recv alive ack";
		return 0;
	}
	return 0;
}

int Amtpca::RecvCmdCallback(void * msg)
{
	JwumqMessage * recv_msg = new JwumqMessage((JwumqMessage *)msg);
	if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
	{
		uint32_t msg_sn = recv_msg->body.sn();
		unique_ptr<JwumqMessage> ack_msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_ack, &msg_sn, sizeof(msg_sn)));
		if(cmd_handle != nullptr)
		{
			jwumq_send(cmd_handle, ack_msg.get());
			JLOG(INFO) << "Send Cmd ack, sn = " << msg_sn;
		}
		
		amtpap::CmdPrimitive resp;
		resp.ParseFromArray(recv_msg->RawData(),recv_msg->RawDataLen());

		JLOG(INFO) << "Recv Cmd, sn = " << resp.serial_number();
		
		AMTP_CMD_ENUM cmd = AMTP_CMD_ENUM::reserve;
		
		if(resp.msg_id() == amtpap::MsgID::LOGIN_RESP_V1)
		{
			RecvLoginResp(recv_msg);
			/*if (recv_mutex_map.find(AMTP_CMD_ENUM::login_resp) != recv_mutex_map.end()
				&& recv_cv_map.find(AMTP_CMD_ENUM::login_resp) != recv_cv_map.end())
			{
				std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::login_resp]);
				recv_cv_map[AMTP_CMD_ENUM::login_resp]->notify_all();
			}*/
			cmd = AMTP_CMD_ENUM::login_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::LOGOUT_RESP_V1)
		{
			RecvLogoutResp(recv_msg);
			cmd = AMTP_CMD_ENUM::logout_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::CONFIG_RESP_V1)
		{
			RecvConfigResp(recv_msg);
			cmd = AMTP_CMD_ENUM::config_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::MODULE_CONF_RESP_V1)
		{
			RecvModuleConfResp(recv_msg);
			cmd = AMTP_CMD_ENUM::module_conf_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::REPORT_EVENT_RESP_V1)
		{
			RecvEventResp(recv_msg);
			cmd = AMTP_CMD_ENUM::report_event_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::ALARM_RESP_V1)
		{
			RecvAlarmResp(recv_msg);
			cmd = AMTP_CMD_ENUM::alarm_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::ALARM_CLEAR_RESP_V1)
		{
			RecvAlarmClearResp(recv_msg);
			cmd = AMTP_CMD_ENUM::alarm_clear_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::GPS_INFO_RESP_V1)
		{
			RecvGpsInfoResp(recv_msg);
			cmd = AMTP_CMD_ENUM::gps_info_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::STATUS_INFO_RESP_V1)
		{
			RecvStatusInfoResp(recv_msg);
			cmd = AMTP_CMD_ENUM::status_info_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::CONFIG_DATA_V1)
		{
			RecvConfigData(recv_msg);
//			if (recv_mutex_map.find(AMTP_CMD_ENUM::status_info_resp) != recv_mutex_map.end()
//				&& recv_cv_map.find(AMTP_CMD_ENUM::status_info_resp) != recv_cv_map.end())
//			{
//				std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::status_info_resp]);
//				recv_cv_map[AMTP_CMD_ENUM::status_info_resp]->notify_all();
//			}
		}
		else if(resp.msg_id() == amtpap::MsgID::CONFIG_NOTIFY_V1)
		{
			RecvConfigNotify(recv_msg);
		}
		else if(resp.msg_id() == amtpap::MsgID::RESTART_CMD_V1)
		{
			RecvRestartCmd(recv_msg);
		}
		else if(resp.msg_id() == amtpap::MsgID::UPLOAD_FILE_RESP_V1)
		{
			RecvUploadFileResp(recv_msg);
			cmd = AMTP_CMD_ENUM::upload_file_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::UPLOAD_EOF_RESP_V1)
		{
			RecvUploadEofResp(recv_msg);
			cmd = AMTP_CMD_ENUM::upload_eof_resp;
		}
		else if(resp.msg_id() == amtpap::MsgID::QUERY_DATA_V1)
		{
			RecvQueryData(recv_msg);
		}
		else
		{
			
		}
		
		if (recv_mutex_map.find(cmd) != recv_mutex_map.end()
			&& recv_cv_map.find(cmd) != recv_cv_map.end())
		{
			std::unique_lock<std::mutex> lock(*recv_mutex_map[cmd]);
			recv_cv_map[cmd]->notify_all();
		}
	}
	else if(recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data_ack))
	{
		std::unique_lock<std::mutex> ack_lock(ack_mutex);
		uint32_t ack_sn = 0;
		memcpy(&ack_sn, recv_msg->RawData(), sizeof(ack_sn));
		ack_cv.notify_all();
		
		JLOG(INFO) << "Command mq recv msg ack, sn = " << ack_sn;
		return 0;
	}
	else if (recv_msg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::private_alive_resp))
	{
		JLOG(INFO) << "Cmd recv alive ack";
		return 0;
	}

	return 0;
}

int Amtpca::WaitForCmd(uint32_t cmd, void * s, uint32_t timeout)
{
	JLOG(INFO) << "WaitForCmd, cmd = " << cmd;
	std::unique_lock<std::mutex> wait_lock(wait_mutex);
	if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::login_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::login_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::login_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::login_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::login_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &login_resp_tmp_s, sizeof(LOGIN_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::logout_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::logout_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::logout_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::logout_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::logout_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			JLOG(INFO) << "LIB_AMTPA_TIMEOUT";
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &logout_resp_tmp_s, sizeof(LOGOUT_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::module_conf_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::module_conf_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::module_conf_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::module_conf_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &module_conf_resp_tmp_s, sizeof(MODULE_CONF_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::report_event_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::report_event_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::report_event_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::report_event_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::report_event_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &report_event_resp_tmp_s, sizeof(REPORT_EVENT_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::alarm_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::alarm_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::alarm_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::alarm_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::alarm_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &alarm_resp_tmp_s, sizeof(ALARM_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::alarm_clear_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::alarm_clear_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::alarm_clear_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::alarm_clear_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::alarm_clear_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &alarm_clear_resp_tmp_s, sizeof(ALARM_CLEAR_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::config_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::config_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::config_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::config_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::config_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &config_resp_tmp_s, sizeof(CONFIG_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::gps_info_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::gps_info_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::gps_info_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::gps_info_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::gps_info_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &gps_info_resp_tmp_s, sizeof(GPS_INFO_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::status_info_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::status_info_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::status_info_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::status_info_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::status_info_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &status_info_resp_tmp_s, sizeof(STATUS_INFO_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::upload_file_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::upload_file_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::upload_file_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::upload_file_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &upload_file_resp_tmp_s, sizeof(UPLOAD_FILE_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::upload_eof_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::upload_eof_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::upload_eof_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::upload_eof_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &upload_eof_resp_tmp_s, sizeof(UPLOAD_EOF_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
	else if(cmd == static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp)
	   && recv_mutex_map.find(AMTP_CMD_ENUM::upload_file_data_resp) != recv_mutex_map.end()
	   && recv_cv_map.find(AMTP_CMD_ENUM::upload_file_data_resp) != recv_cv_map.end())
	{
		std::unique_lock<std::mutex> lock(*recv_mutex_map[AMTP_CMD_ENUM::upload_file_data_resp]);
		if(recv_cv_map[AMTP_CMD_ENUM::upload_file_data_resp]->wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
		{
			return LIB_AMTPA_TIMEOUT;
		}
		memcpy(s, &upload_file_data_resp_tmp_s, sizeof(UPLOAD_FILE_DATA_RESP_STRU));
		return LIB_AMTPA_SUCCESS;
	}
			
	return LIB_AMTPA_WAITFORCMD_ERROR;
}

int Amtpca::SendCmd(uint32_t cmd, void * cmd_s, bool sync, uint32_t timeout)
{
#if defined(_WIN32)
#else
	if (jwumq_send == nullptr)
	{
		return LIB_AMTPA_NO_INIT;
	}
#endif
	//JLOG(INFO) << "Debug Amtpca::SendCmd, cmd = " << cmd;
	switch (cmd)
	{
		case static_cast<uint32_t>(AMTP_CMD_ENUM::login):
		{
			return SendLogin((LOGIN_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::logout):
		{
			return SendLogout(sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf):
		{
			return SendModuleConf((MODULE_CONF_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::report_event):
		{
			return SendEvent((REPORT_EVENT_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::alarm):
		{
			return SendAlarm((ALARM_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::alarm_clear):
		{
			return SendAlarmClear((ALARM_CLEAR_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::config):
		{
			return SendConfig((CONFIG_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::gps_info):
		{
			return SendGpsInfo((GPS_INFO_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::status_info):
		{
			return SendStatusInfo((STATUS_INFO_STRU *)cmd_s, sync, timeout);
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
		case static_cast<uint32_t>(AMTP_CMD_ENUM::restart_cmd_resp):
		{
			return SendRestartCmdResp((RESTART_CMD_RESP_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file):
		{
			return SendUploadFile((UPLOAD_FILE_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof):
		{
			return SendUploadEof((UPLOAD_EOF_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::query_data_resp):
		{
			return SendQueryDataResp((QUERY_DATA_RESP_STRU *)cmd_s, sync, timeout);
			break;
		}
		case static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data):
		{
			return SendUploadFileData((UPLOAD_FILE_DATA_STRU *)cmd_s, sync, timeout);
			break;
		}
		default:
			break;
	}
	return LIB_AMTPA_SEND_ERROR;
}

int Amtpca::SendLogin(LOGIN_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr ||cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_login login;
	msg = login.data((LOGIN_STRU *)s, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	//JLOG(INFO) << "Debug Amtpca::SendLogin.";
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	//JLOG(INFO) << "Debug Amtpca::SendLogin finish.";
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		JLOG(INFO) << "Amtpca::SendLogin ack_cv.wait_for timeout.";
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send login msg, sn = " << msg->body.sn() ;
	
//	SYS_INFO_T info = GetSysInfo();
	return result;
}
void Amtpca::RecvLoginResp(JwumqMessage * recv_msg)
{
	amtp_login_resp resp;
	memset(&login_resp_tmp_s, 0, sizeof(LOGIN_RESP_STRU));
	login_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::login_resp, &login_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv login response, manufacturer = " << login_resp_tmp_s.manufacturer
				<< ", token = " << token
				<< ", result = " << login_resp_tmp_s.result;
}
int Amtpca::SendLogout(bool sync, uint32_t timeout)
{
	if(cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_logout logout;
	msg = logout.data(token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send logout msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvLogoutResp(JwumqMessage * recv_msg)
{
	amtp_logout_resp resp;
	memset(&logout_resp_tmp_s, 0, sizeof(LOGOUT_RESP_STRU));
	logout_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::logout_resp, &logout_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv logout response "
			   << ", token = " << token
			   << ", result = " << logout_resp_tmp_s.result;
}
int Amtpca::SendModuleConf(MODULE_CONF_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_module_conf conf;
	msg = conf.data(s, token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send module cong msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvModuleConfResp(JwumqMessage * recv_msg)
{
	amtp_module_conf_resp resp;
	memset(&module_conf_resp_tmp_s, 0, sizeof(MODULE_CONF_RESP_STRU));
	module_conf_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::module_conf_resp, &module_conf_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv module conf response "
			   << ", token = " << token
			   << ", result = " << module_conf_resp_tmp_s.result;
}
int Amtpca::SendEvent(REPORT_EVENT_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_event event;
	msg = event.data(s, token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send event msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvEventResp(JwumqMessage * recv_msg)
{
	amtp_event_resp resp;
	memset(&report_event_resp_tmp_s, 0, sizeof(REPORT_EVENT_RESP_STRU));
	report_event_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::report_event_resp, &report_event_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv event response "
			   << ", token = " << token
			   << ", result = " << report_event_resp_tmp_s.result;
}
int Amtpca::SendAlarm(ALARM_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_alarm alarm;
	msg = alarm.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send alarm msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvAlarmResp(JwumqMessage * recv_msg)
{
	amtp_alarm_resp resp;
	memset(&alarm_resp_tmp_s, 0, sizeof(ALARM_RESP_STRU));
	alarm_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::alarm_resp, &alarm_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv alarm response "
				<< ", token = " << token
				<< ", result = " << alarm_resp_tmp_s.result;
}
int Amtpca::SendAlarmClear(ALARM_CLEAR_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_alarm_clear alarm_clear;
	msg = alarm_clear.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send alarm clear msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvAlarmClearResp(JwumqMessage * recv_msg)
{
	amtp_alarm_clear_resp resp;
	memset(&alarm_clear_resp_tmp_s, 0, sizeof(ALARM_CLEAR_RESP_STRU));
	alarm_clear_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::alarm_clear_resp, &alarm_clear_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv alarm clear response "
				<< ", token = " << token
				<< ", result = " << alarm_clear_resp_tmp_s.result;
}
int Amtpca::SendConfig(CONFIG_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config config;
	msg = config.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send conig msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvConfigResp(JwumqMessage * recv_msg)
{
	amtp_config_resp resp;
	memset(&config_resp_tmp_s, 0, sizeof(CONFIG_RESP_STRU));
	config_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_resp, &config_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv config response "
				<< ", token = " << token
				<< ", atuid_version = " << config_resp_tmp_s.atuid_version
				<< ", md5 = " << config_resp_tmp_s.md5
				<< ", result = " << config_resp_tmp_s.result;
}
int Amtpca::SendGpsInfo(GPS_INFO_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_gps_info gps_info;
	msg = gps_info.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send gps info msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvGpsInfoResp(JwumqMessage * recv_msg)
{
	amtp_gps_info_resp resp;
	memset(&gps_info_resp_tmp_s, 0, sizeof(GPS_INFO_RESP_STRU));
	gps_info_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::gps_info_resp, &gps_info_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv gps info response "
				<< ", token = " << token
				<< ", result = " << config_resp_tmp_s.result;
}
int Amtpca::SendStatusInfo(STATUS_INFO_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_status_info status_info;
	msg = status_info.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send status info msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvStatusInfoResp(JwumqMessage * recv_msg)
{
	amtp_status_info_resp resp;
	memset(&status_info_resp_tmp_s, 0, sizeof(STATUS_INFO_RESP_STRU));
	status_info_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::status_info_resp, &status_info_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv status info response "
				<< ", token = " << token
				<< ", result = " << config_resp_tmp_s.result;
}
void Amtpca::RecvConfigData(JwumqMessage * recv_msg)
{
	amtp_config_data req;
	CONFIG_DATA_STRU config_data_tmp_s;
	memset(&config_data_tmp_s, 0, sizeof(CONFIG_DATA_STRU));
	bool result = req.data(recv_msg, &config_data_tmp_s);
	if(!result)
	{
		JLOG(INFO) << "Recv config data request, but data format error ";
		return ;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);
	
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_data, &config_data_tmp_s, nullptr);
	}
	if(config_data_tmp_s.config_data != nullptr)
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
int Amtpca::SendConfigDataResp(CONFIG_DATA_RESP_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config_data_resp config_data_resp;
	msg = config_data_resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config data resp msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvConfigNotify(JwumqMessage * recv_msg)
{
	amtp_config_notify req;
	CONFIG_NOTIFY_STRU config_notify_tmp_s;
	memset(&config_notify_tmp_s, 0, sizeof(CONFIG_NOTIFY_STRU));
	bool result = req.data(recv_msg, &config_notify_tmp_s);
	if(!result)
	{
		JLOG(INFO) << "Recv config notify request, but data format error ";
		return ;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);
	
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::config_notify, &config_notify_tmp_s, nullptr);
	}

	JLOG(INFO) << "Recv config notify request "
				<< ", token = " << token
				<< ", atuid_version = " << config_notify_tmp_s.atuid_version
				<< ", packet_count = " << config_notify_tmp_s.packet_count;
}
int Amtpca::SendConfigNotifyResp(CONFIG_NOTIFY_RESP_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_config_notify_resp config_notify_resp;
	msg = config_notify_resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config nofity resp msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvRestartCmd(JwumqMessage * recv_msg)
{
	amtp_restart_cmd req;
	RESTART_CMD_STRU restart_cmd_tmp_s;
	memset(&restart_cmd_tmp_s, 0, sizeof(RESTART_CMD_STRU));
	bool result = req.data(recv_msg, &restart_cmd_tmp_s);
	if(!result)
	{
		JLOG(INFO) << "Recv restart request, but data format error ";
		return ;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);
	
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::restart_cmd, &restart_cmd_tmp_s, nullptr);
	}

	JLOG(INFO) << "Recv restart request "
				<< ", token = " << token
				<< ", ts_sec = " << restart_cmd_tmp_s.ts_sec
				<< ", ts_usec = " << restart_cmd_tmp_s.ts_usec;
}
int Amtpca::SendRestartCmdResp(RESTART_CMD_RESP_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_restart_cmd_resp resp;
	msg = resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send config nofity resp msg, sn = " << msg->body.sn() ;
	return result;
}
int Amtpca::SendUploadFile(UPLOAD_FILE_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_file upload_file;
	msg = upload_file.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload file msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvUploadFileResp(JwumqMessage * recv_msg)
{
	amtp_upload_file_resp resp;
	memset(&upload_file_resp_tmp_s, 0, sizeof(UPLOAD_FILE_RESP_STRU));
	upload_file_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_file_resp, &upload_file_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv upload file response "
				<< ", token = " << token
				<< ", file_name = " << upload_file_resp_tmp_s.file_name
				<< ", file_id = " << upload_file_resp_tmp_s.file_id
				<< ", file_size = " << upload_file_resp_tmp_s.file_size
				<< ", module = " << upload_file_resp_tmp_s.module
				<< ", result = " << upload_file_resp_tmp_s.result;
}
int Amtpca::SendUploadEof(UPLOAD_EOF_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_eof upload_eof;
	msg = upload_eof.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload eof msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvUploadEofResp(JwumqMessage * recv_msg)
{
	amtp_upload_eof_resp resp;
	memset(&upload_eof_resp_tmp_s, 0, sizeof(UPLOAD_EOF_RESP_STRU));
	upload_eof_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_eof_resp, &upload_eof_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv upload eof response "
				<< ", token = " << token
				<< ", file_name = " << upload_eof_resp_tmp_s.file_name
				<< ", file_id = " << upload_eof_resp_tmp_s.file_id
				<< ", packet_count = " << upload_eof_resp_tmp_s.packet_count
				<< ", module = " << upload_eof_resp_tmp_s.module
				<< ", result = " << upload_eof_resp_tmp_s.result;
}
void Amtpca::RecvQueryData(JwumqMessage * recv_msg)
{
	amtp_query_data req;
	QUERY_DATA_STRU query_data_tmp_s;
	memset(&query_data_tmp_s, 0, sizeof(QUERY_DATA_STRU));
	bool result = req.data(recv_msg, &query_data_tmp_s);
	if(!result)
	{
		JLOG(INFO) << "Recv query data request, but data format error ";
		return ;
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	req.token(token);
	
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::query_data, &query_data_tmp_s, nullptr);
	}

	JLOG(INFO) << "Recv query data "
				<< ", token = " << token
				<< ", file_name = " << query_data_tmp_s.file_name
				<< ", file_id = " << query_data_tmp_s.file_id
				<< ", packet_no = " << query_data_tmp_s.packet_no;
}
int Amtpca::SendQueryDataResp(QUERY_DATA_RESP_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || cmd_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_query_data_resp resp;
	msg = resp.data(s, this->token, amtp_conf, cmd_sn++, cmd_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(cmd_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send query data resp msg, sn = " << msg->body.sn() ;
	return result;
}
int Amtpca::SendUploadFileData(UPLOAD_FILE_DATA_STRU * s, bool sync, uint32_t timeout)
{
	if(s == nullptr || data_handle == nullptr)
	{
		return LIB_AMTPA_SEND_PARA_ERROR;
	}
	
	std::unique_lock<std::mutex> send_lock(send_mutex);
	unique_ptr<JwumqMessage> msg = nullptr;
	amtp_upload_file_data upload_file_data;
	msg = upload_file_data.data(s, token, amtp_conf, data_sn++, data_mq_id, "");
	if(msg == nullptr)
	{
		return LIB_AMTPA_UNKNOWN_MSG;
	}
	std::unique_lock<std::mutex> ack_lock(ack_mutex);
	int result = jwumq_send(data_handle, msg.get());
	if(!sync)
	{
		return result;
	}
	if(ack_cv.wait_for(ack_lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
	{
		return LIB_AMTPA_TIMEOUT;
	}
	JLOG(INFO) << "Jwumq send upload file data msg, sn = " << msg->body.sn() ;
	return result;
}
void Amtpca::RecvUploadFileDataResp(JwumqMessage * recv_msg)
{
	amtp_upload_file_data_resp resp;
	memset(&upload_file_data_resp_tmp_s, 0, sizeof(UPLOAD_FILE_DATA_RESP_STRU));
	upload_file_data_resp_tmp_s = resp.data(recv_msg);
	if(cmd_recv_callback != nullptr)
	{
		cmd_recv_callback(AMTP_CMD_ENUM::upload_file_data_resp, &upload_file_data_resp_tmp_s, nullptr);
	}
	memset(token, 0, sizeof(TOKEN_BUFFER_SIZE));
	resp.token(token);
	JLOG(INFO) << "Recv upload file data response "
				<< ", token = " << token
				<< ", file_id = " << upload_file_data_resp_tmp_s.file_id
				<< ", packet_no = " << upload_file_data_resp_tmp_s.packet_no
				<< ", result = " << upload_file_data_resp_tmp_s.result;
}
////
////SYS_INFO_T Amtpca::GetSysInfo()
////{
////	SYS_INFO_T info_s;
////	unique_ptr<SysInfo> info = nullptr;
////	info = make_unique<LinuxInfo>();
////
////	#if (defined _WIN32 || defined _WIN64)
////	info = make_unique<WinInfo>();
////	fprintf(stderr, "_WIN32 || defined _WIN64!\n");
////	#else
////	info = make_unique<LinuxInfo>();
////	fprintf(stderr, "__linux__!\n");
////	#endif
////	if(info != nullptr)
////	{
////		info_s = info->Get();
//////		fprintf(stderr, "GetSysInfo, system:%s, cpu_id:%s, hard_disk:%s!\n"
//////				, info_s.system.c_str(), info_s.cpu_id.c_str(), info_s.hard_disk.c_str());
////	}
////
////	return info_s;
////}
