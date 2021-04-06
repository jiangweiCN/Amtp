//
//  test_main.cpp
//  Amtp
//
//  Created by jiangwei on 2019/12/27.
//  Copyright © 2019 jiangwei. All rights reserved.
//

#include <stdio.h>
#include <cstring>
#include <iostream>
#include <dlfcn.h>
#include <thread>
#include <unistd.h>
#include <sys/time.h>
#include <memory>
#include <string>
#include <sys/time.h>
#include <zlib.h>
#include "/home/libdev/zmq/include/zmq.h"
#include "../jwumq/jwumq_define.h"
#include "../amtpa_protocol/amtpap_msg.pb.h"
#include "libamtpa_define.h"
#include "../jwumq/jwumq_message.hpp"

using namespace std;

int mode = 0;
char proc_id[MAX_IDENTITY_BUF_SIZE] = {0};
void *context;
constexpr auto TOKEN_BUFFER_SIZE = 128;
int cmdprimitive_sn = 0;

void PrintHelp(const char* prog);
void ParseArg(int argc, char *argv[]);

string Time();


#define LIB_JWUMQ_NAME "libjwumq.so"

using LIB_JWUMQ_VERSION = char* (*)();
using LIB_JWUMQ_DEALER_LOAD = void* (*)();
using LIB_JWUMQ_SETUP = int (*)(void *, JWUMQ_SETUP_CONF_T * , JWUMQ_RECV_CALLBACK );
using LIB_JWUMQ_SEND = int (*)(void *, void * );
using LIB_JWUMQ_RELEASE = void (*)(void *);


class LibjwumqDealerClientTest
{
public:
	void Start(string name, int type);
private:
	int RecvCallback(void * recv_msg);
	void TestThread(string name);
private:
	string name;
	int type;

};
void LibjwumqDealerClientTest::Start(string name, int type)
{
	this->type = type;
	this->name = name;
	thread t1(&LibjwumqDealerClientTest::TestThread, this, name);
	t1.join();
}
void LibjwumqDealerClientTest::TestThread(string name)
{
	fprintf(stderr, "%s:------------------LibjwumqTest   %s------------------\n", Time().c_str(), name.c_str());
	void *handle;
	LIB_JWUMQ_VERSION lib_version = NULL;
	LIB_JWUMQ_DEALER_LOAD lib_dealer_load = NULL;
	LIB_JWUMQ_SETUP lib_jwumq_setup = NULL;
	LIB_JWUMQ_SEND lib_jwumq_send = NULL;
	LIB_JWUMQ_RELEASE lib_jwumq_release = NULL;
	
	handle = dlopen(LIB_JWUMQ_NAME, RTLD_NOW);
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray %s error:%s\n", LIB_JWUMQ_NAME, dlerror());
		return ;
	}
	
	lib_version = (LIB_JWUMQ_VERSION)dlsym(handle, "jwumq_version");
	lib_dealer_load = (LIB_JWUMQ_DEALER_LOAD)dlsym(handle, "jwumq_dealer_load");
	lib_jwumq_setup = (LIB_JWUMQ_SETUP)dlsym(handle, "jwumq_setup");
	lib_jwumq_send = (LIB_JWUMQ_SEND)dlsym(handle, "jwumq_send");
	lib_jwumq_release = (LIB_JWUMQ_RELEASE)dlsym(handle, "jwumq_release");
	
	if(lib_version == NULL)
	{
		fprintf(stderr, "lib_version failed\n");
		return;
	}
	fprintf(stderr, "%s:%s Version = %s\n", Time().c_str(), name.c_str(), lib_version());
	
	void * client_handle = lib_dealer_load();
	fprintf(stderr, "%s:%s client handle = %ld\n", Time().c_str(), name.c_str(), reinterpret_cast<unsigned long>( client_handle ));
	
	DEALER_CONF_T conf_t;
	string address = "tcp://10.211.55.9:12120";
	memcpy(conf_t.address, address.c_str(), address.length());
	conf_t.address[address.length()] = 0;
	memcpy(conf_t.identity, name.c_str(), name.length());
	conf_t.identity[name.length()] = 0;
	conf_t.linger = 1;
	conf_t.read_timeout = 5000;
	conf_t.bind = false;
	
	int result = lib_jwumq_setup(client_handle, &conf_t, std::bind(&LibjwumqDealerClientTest::RecvCallback, this
									, std::placeholders::_1));
	fprintf(stderr, "%s:%s lib_jwumq_setup(%d)!\n", Time().c_str(), name.c_str(), result);
	
	sleep(1);
	int index = 0;
	while (index < 10)
	{
		if(type == 2)
		{
			char raw_data[128] = {0};
			sprintf (raw_data, "%s----------%d", name.c_str(), index++);
			
			unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data, name, "", raw_data, (int)strlen(raw_data)));
			result = lib_jwumq_send(client_handle, msg.get());
			fprintf(stderr, "%s:%s send(%d) \"%s\"!\n", Time().c_str(), name.c_str(), result, (char*)msg->RawData());
			
		}
		else if(type == 4)
		{
			char buf[128] = {0};
			sprintf (buf, "%s----------%d", name.c_str(), index++);

			char esn_buf[128] = {0};
			sprintf (esn_buf, "this is client tester.");
			
			amtpap::LoginV1 login;
			
//			login.set_cmd_ip("127.0.0.1");
//			login.set_cmd_port(5570);
			login.set_box_id("00000000");
			login.set_pwd("ffffffff");
			login.set_manufactor("cmdi");
			login.set_begin_time(0);
			login.set_end_time(0);
			login.set_release_id(0xff);
			login.set_jwumq_id("cmdi_test_client_1");
//			login.set_conf_version(12);

			size_t login_len = login.ByteSizeLong();
			unique_ptr<char[]> login_buf(new char[login_len]());
			login.SerializeToArray(login_buf.get(),login_len);

			amtpap::CmdPrimitive request;
			request.set_type(amtpap::MsgType::PUBLIC);
			request.set_serial_number(index);
			request.set_msg_id(amtpap::MsgID::LOGIN_V1);
			request.set_direction(1);
			request.set_payload_data(login_buf.get(), login_len);

			size_t length = request.ByteSizeLong();
			unique_ptr<char[]> raw_data(new char[length]());
			request.SerializeToArray(raw_data.get(),length);

			unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, raw_data.get(), (int)length);
			result = lib_jwumq_send(client_handle, msg.get());

			fprintf(stderr, "%s:%s send(%d, %d) %d!\n", Time().c_str(), name.c_str(), (int)length, result, index);
		}
		usleep(1000);
	}
	sleep(1);
	lib_jwumq_release(client_handle);
	fprintf(stderr, "%s:%s end!\n", Time().c_str(), name.c_str());
}
int LibjwumqDealerClientTest::RecvCallback(void * recv_msg)
{
	if(type == 2)
	{
		JwumqMessage * jmsg = (JwumqMessage *)recv_msg;
		fprintf(stderr, "%s:%s recv(%d) \"%s\", src_id = %s, des_id = %s, sn = %d\n"
				, Time().c_str(), name.c_str(), jmsg->RawDataLen(), (char *)jmsg->RawData()
				, jmsg->body.src_id().c_str(), jmsg->body.des_id().c_str(), jmsg->body.sn());
		
//		unsigned char *mem = (unsigned char *)recv_msg;
//		int offset = 0;
//		JWUMQ_MSG_HEADER_T header;
//		memcpy(&header, mem, sizeof(JWUMQ_MSG_HEADER_T));
//		fprintf(stderr, "header size = %lu\n", sizeof(JWUMQ_MSG_HEADER_T));
//		fprintf(stderr, "command = %d, %d\n", (uint)jmsg->body.command(), (uint)header.command);
//		fprintf(stderr, "ts_sec = %d, %d\n", (uint)jmsg->body.ts_sec, (uint)header.ts_sec);
//		fprintf(stderr, "ts_us = %d, %d\n", jmsg->body.ts_us, header.ts_us);
//		fprintf(stderr, "sn = %d, %d\n", jmsg->body.sn, header.sn);
//		fprintf(stderr, "src_id = %s, %s\n", jmsg->body.src_id, header.src_id);
//		fprintf(stderr, "des_id = %s, %s\n", jmsg->body.des_id, header.des_id);
//		offset += sizeof(JWUMQ_MSG_HEADER_T);
//
//		uint32_t raw_data_len = 0;
//		memcpy(&raw_data_len, mem + offset, sizeof(uint32_t));
//		fprintf(stderr, "raw_data_len = %d\n", raw_data_len);
//		offset += sizeof(uint32_t);
//		offset += 4;
//
//		uint8_t *raw_data_ptr = 0;
//		fprintf(stderr, "pointer size = %lu, %02X %02X %02X %02X %02X %02X %02X %02X\n", sizeof(raw_data_ptr), mem[offset+0], mem[offset+1], mem[offset+2], mem[offset+3], mem[offset+4], mem[offset+5], mem[offset+6], mem[offset+7]);
//
//		memcpy(&raw_data_ptr, mem + offset, sizeof(raw_data_ptr));
//		fprintf(stderr, "raw_data_prt = %p\n", raw_data_ptr);
//		offset += sizeof(raw_data_ptr);
//
//		uint32_t msg_data_len = 0;
//		memcpy(&msg_data_len, mem + offset, sizeof(uint32_t));
//		fprintf(stderr, "msg_data_len = %d\n", msg_data_len);
//		offset += sizeof(uint32_t);
//
//		uint8_t *raw_data = new uint8_t[raw_data_len + 1];
//		memcpy(raw_data, raw_data_ptr, raw_data_len);
//		raw_data[raw_data_len] = 0;
//		fprintf(stderr, "raw_data = %s\n", raw_data);
//		delete[] raw_data;
//
//		int class_size = sizeof(JwumqMessage);
//		string mem_string = "";
//		for (int b = 0; b < class_size; b++)
//		{
//			char m[8];
//			sprintf(m, " %02X", mem[b]);
//			mem_string.append(m);
//		}
//		fprintf(stderr, "memery [%s]\n", mem_string.c_str());
		
	}
	else if(type == 4)
	{
		JwumqMessage * jmsg = (JwumqMessage *)recv_msg;
		
		amtpap::CmdPrimitive resp;
		resp.ParseFromArray(jmsg->RawData(),jmsg->RawDataLen());

		amtpap::LoginRespV1 login_resp;
		login_resp.ParseFromArray(resp.payload_data().c_str(), resp.payload_data().size());

		fprintf(stderr, "%s:%s recv resp, CmdPrimitive type = %d, serial_number = %d, msg_id = %d, direction = %d\n LoginRespV1 token = %s, result = %d\n"
		, Time().c_str(), name.c_str(), resp.type(), resp.serial_number(), resp.msg_id(), resp.direction(),  login_resp.token().c_str(), login_resp.result());
	}
	
	return 0;
}

class LibjwumqDealerServerTest
{
public:
	void Start(string name, int type);
private:
	int RecvCallback(void * recv_msg);
	void TestThread(string name);
private:
	string name;
	int type;
	void * server_handle;
	LIB_JWUMQ_VERSION lib_version ;
	LIB_JWUMQ_DEALER_LOAD lib_dealer_load;
	LIB_JWUMQ_SETUP lib_jwumq_setup;
	LIB_JWUMQ_SEND lib_jwumq_send ;
	LIB_JWUMQ_RELEASE lib_jwumq_release;

};
void LibjwumqDealerServerTest::Start(string name, int type)
{
	this->type = type;
	this->name = name;
	thread t1(&LibjwumqDealerServerTest::TestThread, this, name);
	t1.join();
}
void LibjwumqDealerServerTest::TestThread(string name)
{
	fprintf(stderr, "%s:------------------LibjwumqTest   %s------------------\n", Time().c_str(), name.c_str());
	void *handle;
	lib_version = NULL;
	lib_jwumq_setup = NULL;
	lib_jwumq_send = NULL;
//	lib_jwumq_release = NULL;
	
	handle = dlopen(LIB_JWUMQ_NAME, RTLD_NOW);
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray %s error:%s\n", LIB_JWUMQ_NAME, dlerror());
		return ;
	}
	
	lib_version = (LIB_JWUMQ_VERSION)dlsym(handle, "jwumq_version");
	lib_dealer_load = (LIB_JWUMQ_DEALER_LOAD)dlsym(handle, "jwumq_dealer_load");
	lib_jwumq_setup = (LIB_JWUMQ_SETUP)dlsym(handle, "jwumq_setup");
	lib_jwumq_send = (LIB_JWUMQ_SEND)dlsym(handle, "jwumq_send");
	lib_jwumq_release = (LIB_JWUMQ_RELEASE)dlsym(handle, "jwumq_release");
	
	if(lib_version == NULL)
	{
		fprintf(stderr, "lib_version failed\n");
		return;
	}
	fprintf(stderr, "%s:%s Version = %s\n", Time().c_str(), name.c_str(), lib_version());
	server_handle = lib_dealer_load();
	fprintf(stderr, "%s:%s server handle = %ld\n", Time().c_str(), name.c_str(), reinterpret_cast<unsigned long>( server_handle ));
	
	DEALER_CONF_T conf_t;
	string address = "tcp://127.0.0.1:5581";
	memcpy(conf_t.address, address.c_str(), address.length());
	conf_t.address[address.length()] = 0;
	memcpy(conf_t.identity, name.c_str(), name.length());
	conf_t.identity[name.length()] = 0;
	conf_t.linger = 0;
	conf_t.read_timeout = 5000;
	conf_t.bind = false;
	
	int result = lib_jwumq_setup(server_handle, &conf_t, std::bind(&LibjwumqDealerServerTest::RecvCallback, this
									, std::placeholders::_1));
	fprintf(stderr, "%s:%s lib_jwumq_setup(%d)!\n", Time().c_str(), name.c_str(), result);

	while(true)
	{
		sleep(10);
	}
	lib_jwumq_release(server_handle);
//	fprintf(stderr, "%s:%s end!\n", Time().c_str(), name.c_str());
}
int LibjwumqDealerServerTest::RecvCallback(void * recv_msg)
{
	if(type == 1)
	{
		JwumqMessage * jmsg = (JwumqMessage *)recv_msg;
		fprintf(stderr, "%s:%s recv(%d) \"%s\", src_id = %s, des_id = %s\n"
				, Time().c_str(), name.c_str(), jmsg->RawDataLen(), (char *)jmsg->RawData()
				, jmsg->body.src_id().c_str(), jmsg->body.des_id().c_str());

		char raw_data[128] = {0};
		sprintf (raw_data, "%s----------%s", (char *)jmsg->RawData(), name.c_str());
//		memcpy(msg->body.des_id, jmsg->body.src_id, strlen(jmsg->body.src_id));
		if(jmsg->body.command() == static_cast<uint32_t>(JWUMQ_COMMAND_ENUM::public_data))
		{
			unique_ptr<JwumqMessage> msg(new JwumqMessage(JWUMQ_COMMAND_ENUM::public_data_ack, name, jmsg->body.src_id(), raw_data, (int)strlen(raw_data)));
			int result = lib_jwumq_send(server_handle, msg.get());
			fprintf(stderr, "%s:%s send(%d) \"%s\"!\n", Time().c_str(), name.c_str(), result, (char*)msg->RawData());
		}
	}
	else if(type == 3)
	{
		JwumqMessage * jmsg = (JwumqMessage *)recv_msg;
		amtpap::CmdPrimitive req;
		req.ParseFromArray(jmsg->RawData(), jmsg->RawDataLen());

		amtpap::LoginV1 login;
		login.ParseFromArray(req.payload_data().c_str(), req.payload_data().size());

		fprintf(stderr, "%s:%s recv request, CmdPrimitive type = %d, serial_number = %d, msg_id = %d, direction = %d\n box_id = %s, pwd = %s, manufactor = %s, "\
				"begin_time = %ld, end_time = %ld, release_id = %d, jwumq_id = %s\n"
				, Time().c_str(), name.c_str(), req.type(), req.serial_number(), req.msg_id(), req.direction()
				, login.box_id().c_str(), login.pwd().c_str(), login.manufactor().c_str()
				, login.begin_time(), login.end_time(), login.release_id(), login.jwumq_id().c_str());

		char buf[128] = {0};
		sprintf (buf, "%s----------%s", login.box_id().c_str(), name.c_str());

		amtpap::LoginRespV1 login_resp;
		login_resp.set_result(0);
		login_resp.set_token(buf, strlen(buf));

		size_t login_resp_len = login_resp.ByteSizeLong();
		unique_ptr<char[]> login_resp_buf(new char[login_resp_len]());
		login_resp.SerializeToArray(login_resp_buf.get(), (int)login_resp_len);


		amtpap::CmdPrimitive resp;
		resp.set_type(amtpap::MsgType::PUBLIC);
		resp.set_serial_number(req.serial_number());
		resp.set_msg_id(amtpap::MsgID::LOGIN_RESP_V1);
		resp.set_direction(0);
		resp.set_payload_data(login_resp_buf.get(), login_resp_len);

		size_t length = resp.ByteSizeLong();
		unique_ptr<char[]> send_buf(new char[length]());
		resp.SerializeToArray(send_buf.get(), (int)length);

		unique_ptr<JwumqMessage> msg = make_unique<JwumqMessage>(JWUMQ_COMMAND_ENUM::public_data, "" , jmsg->body.src_id(), send_buf.get(), (int)length);
//		memcpy(msg->body.des_id, jmsg->body.src_id, strlen(jmsg->body.src_id));
		int result = lib_jwumq_send(server_handle, msg.get());
		fprintf(stderr, "%s:%s send(%d, %d) %d!\n", Time().c_str(), name.c_str(), (int)length, result, req.serial_number());
	}
	
	return 0;
}


using LIB_AMTPSA_VERSION = char* (*)();
using LIB_AMTPSA_LOAD = void* (*)();
using LIB_AMTPSA_CONNECTION = int (*)(void *, const char * , const char *, const char * );
using LIB_AMTPSA_SETLIMIT = int (*)(void *, int);
using LIB_AMTPSA_SENDMSG = int (*)(void *, uint32_t, const char * , void * , int );
using LIB_AMTPSA_READMSG = int (*)(void *, char * , void * , int );
using LIB_AMTPSA_WAITFORMSG = int (*)(void *, int *, int);
using LIB_AMTPSA_RELEASE = void (*)(void *);
constexpr auto config_file_name = "test_scheme_templet.xml";

class LibAmtpsaTest
{
public:
	void Start(string name, int type);
private:
	void TestThread(string name);
	void RecvLogin(string, string, int);
	void RecvLogout(string, string, int);
	void RecvEvent(string, string, int);
	void RecvAlarm(string, string, int);
	void RecvConfig(string, string, int, string &);
	void RecvAlarmClear(string, string, int);
	void RecvGpsInfo(string, string, int);
	void RecvStatusInfo(string, string, int);
	void SendConfigData(string, string, int packet_no);
	void RecvConfigDataResp(string, string, int);
	void SendConfigNotify(string);
	void RecvConfigNotifyResp(string, string, int, string &);
	void SendRestartCmd(string);
	void RecvRestartCmdResp(string, string, int);
	void RecvUploadFile(string, string, int);
	void RecvUploadEof(string, string, int);
	void SendQueryData(string);
	void RecvQueryDataResp(string, string, int);
	void RecvUploadFileData(string, string, int);
	void RecvModuleConf(string, string, int);
private:
	string name;
	int type;
	LIB_AMTPSA_VERSION lib_version ;
	LIB_AMTPSA_LOAD lib_load;
	LIB_AMTPSA_CONNECTION lib_connection;
	LIB_AMTPSA_SETLIMIT lib_setlimit;
	LIB_AMTPSA_SENDMSG lib_sendmsg;
	LIB_AMTPSA_READMSG lib_readmsg ;
	LIB_AMTPSA_WAITFORMSG lib_waitformsg;
	LIB_AMTPSA_RELEASE lib_release;
	char token[TOKEN_BUFFER_SIZE];
	void * sa_handle;

};
void LibAmtpsaTest::Start(string name, int type)
{
	this->type = type;
	this->name = name;
	sa_handle = NULL;
	thread t1(&LibAmtpsaTest::TestThread, this, name);
	t1.join();
}
void LibAmtpsaTest::TestThread(string name)
{
	fprintf(stderr, "%s:------------------Libamtpsa Test   %s------------------\n", Time().c_str(), name.c_str());
	
	void *so;
	
	lib_version = NULL;
	lib_connection = NULL;
	lib_setlimit = NULL;
	lib_sendmsg = NULL;
	lib_readmsg = NULL;
	lib_waitformsg = NULL;
	lib_release = NULL;
	
	so = dlopen("libamtpsa.so", RTLD_NOW);
	if (so == NULL)
	{
		fprintf(stderr, "Failed to open libaray libamtpsa.so error:%s\n", dlerror());
		return ;
	}
	
	lib_version = (LIB_AMTPSA_VERSION)dlsym(so, "amtpsa_version");
	lib_load = (LIB_AMTPSA_LOAD)dlsym(so, "amtpsa_load");
	lib_connection = (LIB_AMTPSA_CONNECTION)dlsym(so, "amtpsa_connection");
	lib_setlimit = (LIB_AMTPSA_SETLIMIT)dlsym(so, "amtpsa_setlimit");
	lib_sendmsg = (LIB_AMTPSA_SENDMSG)dlsym(so, "amtpsa_sendmsg");
	lib_readmsg = (LIB_AMTPSA_READMSG)dlsym(so, "amtpsa_readmsg");
	lib_waitformsg = (LIB_AMTPSA_WAITFORMSG)dlsym(so, "amtpsa_waitformsg");
	lib_release = (LIB_AMTPSA_RELEASE)dlsym(so, "amtpsa_release");
	
	if(lib_version == NULL)
	{
		fprintf(stderr, "lib_version failed\n");
		return;
	}
	fprintf(stderr, "%s:%s Version = %s\n", Time().c_str(), name.c_str(), lib_version());
	
	sa_handle = lib_load();
	fprintf(stderr, "%s:%s sa_handle = %lu\n", Time().c_str(), name.c_str(), (unsigned long)sa_handle);
	
	char* cmd_address = (char*)"tcp://127.0.0.1:5581";
	char* data_address = (char*)"tcp://127.0.0.1:5580";
	int result = lib_connection(sa_handle, name.c_str(), cmd_address, data_address);
	fprintf(stderr, "%s:%s lib_amtpsa_connection(%d)!\n", Time().c_str(), name.c_str(), result);
	if(result != LIB_AMTPA_SUCCESS)
	{
		return ;
	}
	lib_setlimit(sa_handle, 100);

	int index = 0;
	while(index < 100000000)
	{
		int msg_len = 0;
		int result = lib_waitformsg(sa_handle, &msg_len, 10000);
		
		index++;
		
		if(result > 0 && msg_len > 0)
		{
			fprintf(stderr, "%s:%s amtpsa has a msg(len=%d), remain= %d!\n", Time().c_str(), name.c_str(), msg_len, result);
			unique_ptr<char[]> recv_buf(new char[msg_len]());
			char src_id[MAX_IDENTITY_BUF_SIZE] = {0};
			lib_readmsg(sa_handle, src_id, recv_buf.get(), msg_len);
			fprintf(stderr, "%s:%s recv(%s, %d)!\n", Time().c_str(), name.c_str(), src_id, msg_len);
			
			amtpap::CmdPrimitive req;
			req.ParseFromArray(recv_buf.get(), msg_len);
			
			fprintf(stderr, "%s:%s recv request, CmdPrimitive type = %d, serial_number = %d, msg_id = %d, direction = %d\n", Time().c_str(), name.c_str(), req.type(), req.serial_number(), req.msg_id(), req.direction());

			if(req.msg_id() == amtpap::MsgID::LOGIN_V1)
			{
				RecvLogin(src_id, req.payload_data(), (int)req.payload_data().size());
			}
			else if(req.msg_id() == amtpap::MsgID::MODULE_CONF_V1)
			{
				RecvModuleConf(src_id, req.payload_data(), (int)req.payload_data().size());
			}
			else if(req.msg_id() == amtpap::MsgID::CONFIG_V1)
			{
				string atuid_version = "";
				RecvConfig(src_id, req.payload_data(), (int)req.payload_data().size(), atuid_version);
				usleep(100 * 1000);
				SendConfigData(src_id, atuid_version, 1);
			}
			else if(req.msg_id() == amtpap::MsgID::CONFIG_DATA_RESP_V1)
			{
				RecvConfigDataResp(src_id, req.payload_data(), (int)req.payload_data().size());
			}
			else if(req.msg_id() == amtpap::MsgID::UPLOAD_FILE_V1)
			{
				RecvUploadFile(src_id, req.payload_data(), (int)req.payload_data().size());
			}
			else if(req.msg_id() == amtpap::MsgID::UPLOAD_FILE_DATA_V1)
			{
				RecvUploadFileData(src_id, req.payload_data(), (int)req.payload_data().size());
			}
			else if(req.msg_id() == amtpap::MsgID::UPLOAD_EOF_V1)
			{
				RecvUploadEof(src_id, req.payload_data(), (int)req.payload_data().size());
				usleep(100 * 1000);
				SendQueryData(src_id);
			}
			else if(req.msg_id() == amtpap::MsgID::LOGOUT_V1)
			{
				RecvLogout(src_id, req.payload_data(), (int)req.payload_data().size());
				sleep(1);
//				SendRestartCmd(src_id);
				SendConfigNotify(src_id);
			}
			else if(req.msg_id() == amtpap::MsgID::CONFIG_NOTIFY_RESP_V1)
			{
				string atuid_version = "";
				RecvConfigNotifyResp(src_id, req.payload_data(), (int)req.payload_data().size(), atuid_version);
				usleep(100 * 1000);
				SendConfigData(src_id, atuid_version, 1);
			}
			else if(req.msg_id() == amtpap::MsgID::QUERY_DATA_RESP_V1)
			{
				RecvQueryDataResp(src_id, req.payload_data(), (int)req.payload_data().size());
			}
//			else if(req.msg_id() == amtpap::MsgID::REPORT_EVENT_V1)
//			{
//				RecvEvent(src_id, req.payload_data(), (int)req.payload_data().size());
//			}
//			else if(req.msg_id() == amtpap::MsgID::ALARM_V1)
//			{
//				RecvAlarm(src_id, req.payload_data(), (int)req.payload_data().size());
//			}
//			else if(req.msg_id() == amtpap::MsgID::ALARM_CLEAR_V1)
//			{
//				RecvAlarmClear(src_id, req.payload_data(), (int)req.payload_data().size());
//			}
//			else if(req.msg_id() == amtpap::MsgID::GPS_INFO_V1)
//			{
//				RecvGpsInfo(src_id, req.payload_data(), (int)req.payload_data().size());
//			}
//			else if(req.msg_id() == amtpap::MsgID::STATUS_INFO_V1)
//			{
//				RecvStatusInfo(src_id, req.payload_data(), (int)req.payload_data().size());
//				sleep(1);
//				SendConfigNotify(src_id);
//			}
//			else if(req.msg_id() == amtpap::MsgID::RESTART_CMD_RESP_V1)
//			{
//				RecvRestartCmdResp(src_id, req.payload_data(), (int)req.payload_data().size());
//			}
		}
		else if(result == LIB_AMTPA_TIMEOUT)
		{
			fprintf(stderr, "%s:%s amtpsa wait for msg time out(%d)!\n", Time().c_str(), name.c_str(), result);
		}
		else if(result == 0)
		{
			fprintf(stderr, "%s:%s amtpsa no msg(%d)!\n", Time().c_str(), name.c_str(), result);
		}
		else
		{
			fprintf(stderr, "%s:%s amtpsa wait for msg error(%d)!\n", Time().c_str(), name.c_str(), result);
		}
	}
//	fprintf(stderr, "%s:%s end!\n", Time().c_str(), name.c_str());
	lib_release(sa_handle);
	dlclose(so);
}

void LibAmtpsaTest::RecvModuleConf(string src_id, string data, int len)
{
	amtpap::ModuleConfV1 req;
	req.ParseFromArray(data.c_str(), len);

	fprintf(stderr, "%s:%s recv module conf cmd token = %s, module_type = %d, chip_manufacturer = %d, \
chip_type = %s, icd_version = %s, imei = %s, phone_number = %s, network_operator = %s \
module_num = %d, module_system = %s, module_factory = %s, imsi = %s, box_id = %s\n",
			Time().c_str(), name.c_str(), req.token().c_str(), req.module_type(), req.chip_manufacturer(), 
			req.chip_type().c_str(), req.icd_version().c_str(), req.imei().c_str(), req.phone_number().c_str(), req.network_operator().c_str(), 
			req.module_num(), req.module_system().c_str(), req.module_factory().c_str(), req.imsi().c_str(), req.box_id().c_str());

	amtpap::ModuleConfRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::MODULE_CONF_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send module conf response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::SendQueryData(string des_id)
{
	amtpap::QueryDataV1 data;
	data.set_token(token, strlen(token));
	data.set_file_name("9177700220190103104005ms1.lte");
	data.set_file_id(123);
	data.set_packet_no(1);

	size_t data_len = data.ByteSizeLong();
	unique_ptr<char[]> data_buf(new char[data_len]());
	data.SerializeToArray(data_buf.get(), (int)data_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::QUERY_DATA_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(data_buf.get(), data_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::query_data), des_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send query data (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, des_id.c_str());
}
void LibAmtpsaTest::RecvQueryDataResp(string src_id, string data, int len)
{
	amtpap::ConfigNotifyRespV1 resp;
	resp.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv restart cmd resp token = %s, result = %d\n"
			, Time().c_str(), name.c_str(), resp.token().c_str(), resp.result());
}
void LibAmtpsaTest::SendRestartCmd(string des_id)
{
	amtpap::ReatartCmdV1 data;
	data.set_token(token, strlen(token));
	data.set_ts_sec(1);
	data.set_ts_usec(2);

	size_t data_len = data.ByteSizeLong();
	unique_ptr<char[]> data_buf(new char[data_len]());
	data.SerializeToArray(data_buf.get(), (int)data_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::RESTART_CMD_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(data_buf.get(), data_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::restart_cmd), des_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send restart cmd (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, des_id.c_str());
}
void LibAmtpsaTest::RecvRestartCmdResp(string src_id, string data, int len)
{
	amtpap::ConfigNotifyRespV1 resp;
	resp.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv restart cmd resp token = %s, result = %d\n"
			, Time().c_str(), name.c_str(), resp.token().c_str(), resp.result());
}
void LibAmtpsaTest::SendConfigNotify(string des_id)
{
	int max_packet_size = 1024 * 10;
	FILE *config_f = fopen(config_file_name, "rb" );
	size_t config_size = 0;
	fseek(config_f,0L,SEEK_END);
	config_size=ftell(config_f);
	fseek(config_f,0L,SEEK_SET);
	fclose(config_f);
	
	int packet_count = (config_size / max_packet_size)
						+ (((int)(config_size % max_packet_size) == 0) ? 0 : 1);
	string md5 = "123456abcdefghijklmnopqrstuvwxyz";
	
	amtpap::ConfigNotifyV1 data;
	data.set_token(token, strlen(token));
//	data.set_version(123);
	data.set_atuid_version("e5d4c3b2a1_321");
	data.set_packet_count(packet_count);
	data.set_md5(md5);

	size_t data_len = data.ByteSizeLong();
	unique_ptr<char[]> data_buf(new char[data_len]());
	data.SerializeToArray(data_buf.get(), (int)data_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::CONFIG_NOTIFY_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(data_buf.get(), data_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::config_notify), des_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send config notify (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, des_id.c_str());
}
void LibAmtpsaTest::RecvConfigNotifyResp(string src_id, string data, int len, string &atuid_version)
{
	amtpap::ConfigNotifyRespV1 resp;
	resp.ParseFromArray(data.c_str(), len);
	atuid_version = resp.atuid_version();
	
	fprintf(stderr, "%s:%s recv config notify resp token = %s, result = %d, atuid_version = %s\n"
			, Time().c_str(), name.c_str(), resp.token().c_str(), resp.result(), resp.atuid_version().c_str());
}
void LibAmtpsaTest::SendConfigData(string des_id, string atuid_version, int packet_no)
{
	int max_packet_size = 1024 * 10;
	FILE *config_f = fopen( config_file_name, "rb" );
	size_t config_size = 0;
	fseek(config_f,0L,SEEK_END);
	config_size=ftell(config_f);
	
	int packet_count = ((int)config_size / max_packet_size)
						+ (((int)(config_size % max_packet_size) == 0) ? 0 : 1);
	if(packet_no > packet_count || packet_no < 1)
	{
		fclose(config_f);
		return;
	}
	
	int read_len = 0;
	if(packet_no == packet_count)
	{
		read_len = (int)config_size - (packet_no - 1) * max_packet_size;
	}
	else
	{
		read_len = max_packet_size;
	}
	fprintf(stderr, "%s:%s config size = %ld, packet count = %d, packet no = %d, data_len = %d!\n"
	, Time().c_str(), name.c_str(), config_size, packet_count, packet_no, read_len);
	
	fseek(config_f, (packet_no - 1) * max_packet_size, SEEK_SET);
	unique_ptr<unsigned char[]> buf(new unsigned char[read_len]());
	fread( buf.get(), 1, read_len, config_f );
	fclose(config_f);
	
	amtpap::ConfigDataV1 data;
	data.set_token(token, strlen(token));
//	data.set_version(100);
	data.set_atuid_version(atuid_version);
	data.set_packet_count(packet_count);
	data.set_packet_no(packet_no);
	data.set_data_length(read_len);
	data.set_config_data(buf.get(), read_len);

	size_t data_len = data.ByteSizeLong();
	unique_ptr<char[]> data_buf(new char[data_len]());
	data.SerializeToArray(data_buf.get(), (int)data_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::CONFIG_DATA_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(data_buf.get(), data_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::config_data), des_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send config data (%d, %d, %s) to %s!\n"
			, Time().c_str(), name.c_str(), (int)length, result, atuid_version.c_str(), des_id.c_str());
}
void LibAmtpsaTest::RecvConfigDataResp(string src_id, string data, int len)
{
	amtpap::ConfigDataRespV1 resp;
	resp.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv config data resp token = %s, atuid_version = %s, packet_no = %d, result = %d\n"
			, Time().c_str(), name.c_str(), resp.token().c_str(), resp.atuid_version().c_str(), resp.packet_no(), resp.result());
	
	usleep(100 * 1000);
	SendConfigData(src_id, resp.atuid_version(), resp.packet_no() + 1);
}
void LibAmtpsaTest::RecvLogin(string src_id, string data, int len)
{
	amtpap::LoginV1 login;
	login.ParseFromArray(data.c_str(), len);

	fprintf(stderr, "%s:%s recv login command box_id = %s, pwd = %s, manufactor = %s, "\
	"begin_time = %ld, end_time = %ld, release_id = %d, jwumq_id = %s\n"
			, Time().c_str(), name.c_str(), login.box_id().c_str()
			, login.pwd().c_str(), login.manufactor().c_str(), login.begin_time(), login.end_time()
			, login.release_id(), login.jwumq_id().c_str());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "%s-----login-----%s", login.box_id().c_str(), name.c_str());
	usleep(10 * 1000);

	amtpap::LoginRespV1 login_resp;
	login_resp.set_token(token, strlen(token));
	login_resp.set_manufactor(login.manufactor());
	login_resp.set_result(0);

	size_t login_resp_len = login_resp.ByteSizeLong();
	unique_ptr<char[]> login_resp_buf(new char[login_resp_len]());
	login_resp.SerializeToArray(login_resp_buf.get(), (int)login_resp_len);

	amtpap::CmdPrimitive resp;
	resp.set_type(amtpap::MsgType::PUBLIC);
	resp.set_serial_number(cmdprimitive_sn++);
	resp.set_msg_id(amtpap::MsgID::LOGIN_RESP_V1);
	resp.set_direction(0);
	resp.set_payload_data(login_resp_buf.get(), login_resp_len);

	size_t length = resp.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	resp.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::login_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send login response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
	
}
void LibAmtpsaTest::RecvLogout(string src_id, string data, int len)
{
	amtpap::LogoutV1 req;
	req.ParseFromArray(data.c_str(), len);

	fprintf(stderr, "%s:%s recv logout command token = %s\n"
			, Time().c_str(), name.c_str(), req.token().c_str());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----logout-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::LogoutRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::LOGOUT_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::logout_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send logout response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvEvent(string src_id, string data, int len)
{
	amtpap::ReportEventV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv event command token = %s, module = %d, event_code = %d, ts_usec = %d, ts_sec = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.module(), req.event_code(), req.ts_usec(), req.ts_sec());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----event-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::ReportEventRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::REPORT_EVENT_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::report_event_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send event response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvAlarm(string src_id, string data, int len)
{
	amtpap::AlarmV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv alarm command token = %s, seq = %d, module = %d, alarm_code = %d, reason_code = %d, ts_sec = %d, ts_usec = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.seq(), req.module(), req.alarm_code(), req.reason_code(), req.ts_sec(), req.ts_usec());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----alarm-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::AlarmRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::ALARM_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::alarm_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send alarm response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvConfig(string src_id, string data, int len, string &atuid_version)
{
	int max_packet_size = 1024 * 10;
	FILE *config_f = fopen(config_file_name, "rb" );
	size_t config_size = 0;
	fseek(config_f,0L,SEEK_END);
	config_size=ftell(config_f);
	fseek(config_f,0L,SEEK_SET);
	fclose(config_f);
		
	int packet_count = (config_size / max_packet_size)
							+ (((int)(config_size % max_packet_size) == 0) ? 0 : 1);
	
	amtpap::ConfigV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv config command token = %s, atuid_version = %s\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.atuid_version().c_str());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----config-----%s", name.c_str());
	usleep(10 * 1000);
	
	string md5 = "abcdefghijklmnopqrstuvwxyz123456";
	amtpap::ConfigRespV1 resp;
	resp.set_token(token, strlen(token));
//	resp.set_version(req.version() + 1);
	resp.set_atuid_version(req.atuid_version());
	atuid_version = req.atuid_version();
	resp.set_result(0);
	resp.set_update(1);
	resp.set_packet_count(packet_count);
	resp.set_md5(md5);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::CONFIG_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::config_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send config response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvAlarmClear(string src_id, string data, int len)
{
	amtpap::AlarmClearV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv alarm clear command token = %s, seq = %d, module = %d, alarm_code = %d, ts_sec = %d, ts_usec = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.seq(), req.module(), req.alarm_code(), req.ts_sec(), req.ts_usec());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----alarm clear-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::AlarmClearRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::ALARM_CLEAR_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::alarm_clear_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send alarm clear response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvGpsInfo(string src_id, string data, int len)
{
	amtpap::GPSInfoV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv gps info command token = %s, longitude = %f, latitude = %f, altitude = %f, speed = %f, heading = %f, ts_sec = %d, ts_usec = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.longitude(), req.latitude(), req.altitude(), req.speed(), req.heading(), req.ts_sec(), req.ts_usec());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----gps info-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::GPSInfoRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::GPS_INFO_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::gps_info_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send gps info response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvStatusInfo(string src_id, string data, int len)
{
	amtpap::StatusInfoV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv gps info command token = %s, power_mode = %d, files_letf = %d, temperature = %f, disk_space = %f, ts_sec = %d, ts_usec = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.power_mode(), req.files_letf(), req.temperature(), req.disk_space(), req.ts_sec(), req.ts_usec());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----status info-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::StatusInfoRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::STATUS_INFO_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::status_info_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send status info response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvUploadFile(string src_id, string data, int len)
{
	amtpap::UploadFileV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv upload file command token = %s, file_name = %s, retransmit = %d, module = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.file_name().c_str(), req.retransmit(), req.module());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----upload file-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::UploadFileRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_file_name(req.file_name());
	resp.set_file_id(12345);
	resp.set_file_size(67890);
	resp.set_module(req.module());
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::UPLOAD_FILE_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send upload file response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvUploadEof(string src_id, string data, int len)
{
	amtpap::UploadEofV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv upload eof command token = %s, file_name = %s, file_id = %d, packet_count = %d, total_size = %d, module = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.file_name().c_str(), req.file_id(), req.packet_count(), req.total_size(), req.module());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----upload file-----%s", name.c_str());
	usleep(10 * 1000);

	amtpap::UploadEofRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_file_name(req.file_name());
	resp.set_file_id(12345);
	resp.set_packet_count(23);
	resp.set_module(req.module());
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::UPLOAD_EOF_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send upload eof response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}
void LibAmtpsaTest::RecvUploadFileData(string src_id, string data, int len)
{
	amtpap::UploadFileDataV1 req;
	req.ParseFromArray(data.c_str(), len);
	
	fprintf(stderr, "%s:%s recv upload file data command token = %s, file_id = %d, packet_no = %d, piece_len = %d, data_len = %d\n"
			, Time().c_str(), name.c_str(), req.token().c_str(), req.file_id(), req.packet_no(), req.piece_len(), req.data_len());
	
	memset(token, 0, TOKEN_BUFFER_SIZE);
	sprintf (token, "-----upload file data-----%s", name.c_str());
	usleep(10 * 1000);
	
	unique_ptr<unsigned char[]> uncom_data(new unsigned char[req.data_len()]());
	int err;

	unsigned long uncom_data_len = req.data_len();
	err = uncompress(uncom_data.get(), &uncom_data_len, (unsigned char*)req.data().c_str(), req.data_len());
	if (err != Z_OK)
	{
		fprintf(stderr, "uncompess error: %d\n", err);
	}
	
	FILE *dtlog_f;
	if(req.packet_no() == 1)
	{
		dtlog_f = fopen( "recv_dtlog.txt", "wb" );
	}
	else
	{
		dtlog_f = fopen( "recv_dtlog.txt", "ab" );
	}
	fwrite(uncom_data.get(), uncom_data_len, 1, dtlog_f);
	fclose(dtlog_f);
	
	
	amtpap::UploadFileDataRespV1 resp;
	resp.set_token(token, strlen(token));
	resp.set_file_id(req.file_id());
	resp.set_packet_no(req.packet_no());
	resp.set_result(0);

	size_t resp_len = resp.ByteSizeLong();
	unique_ptr<char[]> resp_buf(new char[resp_len]());
	resp.SerializeToArray(resp_buf.get(), (int)resp_len);

	amtpap::CmdPrimitive cmdprimitive;
	cmdprimitive.set_type(amtpap::MsgType::PUBLIC);
	cmdprimitive.set_serial_number(cmdprimitive_sn++);
	cmdprimitive.set_msg_id(amtpap::MsgID::UPLOAD_FILE_DATA_RESP_V1);
	cmdprimitive.set_direction(0);
	cmdprimitive.set_payload_data(resp_buf.get(), resp_len);

	size_t length = cmdprimitive.ByteSizeLong();
	unique_ptr<char[]> send_buf(new char[length]());
	cmdprimitive.SerializeToArray(send_buf.get(), (int)length);

	int result = lib_sendmsg(sa_handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp), src_id.c_str(), send_buf.get(), length);
	fprintf(stderr, "%s:%s send upload file data response (%d, %d) to %s!\n", Time().c_str(), name.c_str(), (int)length, result, src_id.c_str());
}

using LIB_AMTPCA_VERSION = char* (*)();
using LIB_AMTPCA_INIT = int (*)(const char * , const char * , LIBAMTPCA_CMD_RECV_CBFUN );
using LIB_AMTPCA_SENDCMD = int (*)(uint32_t, void * , bool , uint32_t );
using LIB_AMTPSA_WAITFORCMD = int (*)(uint32_t, void *, int);
using LIB_AMTPCA_RELEASE = void (*)();

class LibAmtpcaTest
{
public:
	void Start(string name, int type);
private:
	void TestThread(string name);
	void LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void * cmd_s, void* reserve);
	void LoginTest();
	void ModuleConfigTest();
	void ConfigTest();
	void UploadFileRequestTest();
	void UploadFileDataTest();
	void UploadFileEofTest();
	void LogoutTest();
	void PerformanceTest();

private:
	string name;
	int type;
	LIB_AMTPCA_VERSION lib_version ;
	LIB_AMTPCA_INIT lib_init;
	LIB_AMTPCA_SENDCMD lib_sendcmd;
	LIB_AMTPSA_WAITFORCMD lib_waitforcmd;
	LIB_AMTPCA_RELEASE lib_release;

};
void LibAmtpcaTest::Start(string name, int type)
{
	this->type = type;
	this->name = name;
	thread t1(&LibAmtpcaTest::TestThread, this, name);
	t1.join();
}
void LibAmtpcaTest::TestThread(string name)
{
	fprintf(stderr, "%s:------------------Libamtpca Test   %s------------------\n", Time().c_str(), name.c_str());
	
	void *handle;
	
	lib_version = NULL;
	lib_init = NULL;
	lib_sendcmd = NULL;
	lib_waitforcmd = NULL;
	lib_release = NULL;
	
	handle = dlopen("libamtpca.so", RTLD_NOW);
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray libamtpca.so error:%s\n", dlerror());
		return ;
	}
	
	lib_version = (LIB_AMTPCA_VERSION)dlsym(handle, "amtpca_version");
	lib_init = (LIB_AMTPCA_INIT)dlsym(handle, "amtpca_init");
	lib_sendcmd = (LIB_AMTPCA_SENDCMD)dlsym(handle, "amtpca_sendcmd");
	lib_waitforcmd = (LIB_AMTPSA_WAITFORCMD)dlsym(handle, "amtpca_waitforcmd");
	lib_release = (LIB_AMTPCA_RELEASE)dlsym(handle, "amtpca_release");
	
	if(lib_version == NULL)
	{
		fprintf(stderr, "lib_version failed\n");
		return;
	}
	fprintf(stderr, "%s:%s Version = %s\n", Time().c_str(), name.c_str(), lib_version());
	
	if(lib_init == NULL)
	{
		fprintf(stderr, "lib_init failed\n");
		return;
	}
	int result = -1;
	
	if (name == "client_p")
	{
		result = lib_init("dev.license", "log", std::bind(&LibAmtpcaTest::LibamtpcaCmdRecvCbfun, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else
	{
		result = lib_init("dev.license", NULL, std::bind(&LibAmtpcaTest::LibamtpcaCmdRecvCbfun, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	
	fprintf(stderr, "%s:%s Init result = %d\n", Time().c_str(), name.c_str(), result);
	if(result != 0)
	{
		return;
	}

	while(1)
	{
		char choice[64] = {0};
		printf("=======================================================================\n");
		printf("please input your choice\n");
		printf("1: Login\n");
		printf("2: Module conf report\n");
		printf("3: Config request\n");
		printf("4: Upload file request\n");
		printf("5: Upload file data\n");
		printf("6: Upload file eof\n");
		printf("7: Logout\n");
		printf("8: Performance test\n");
		printf("0: Exit test\n");
		printf("Select:");
		scanf("%s",choice);

		int select = atoi(choice);

		if(select == 1)
		{
			LoginTest ();
		}
		else if(select == 2)
		{
			ModuleConfigTest();
		}
		else if (select == 3)
		{
			ConfigTest();
		}
		else if (select == 4)
		{
			UploadFileRequestTest();
		}
		else if (select == 5)
		{
			UploadFileDataTest();
		}
		else if (select == 6)
		{
			UploadFileEofTest();
		}
		else if (select == 7)
		{
			LogoutTest();
		}
		else if (select == 8)
		{
			PerformanceTest();
		}
		else if (select == 0)
		{
			break;
		}
		else
		{
			printf("***********Your choice is invalid!\n");
		}
	}
	fprintf(stderr, "%s:%s end!\n", Time().c_str(), name.c_str());
	
	lib_release();
}
void LibAmtpcaTest::LoginTest ()
{
	char manufacturer_pwd[64] = {0};
	printf("please input manufacturer pwd: ");
	scanf("%s", manufacturer_pwd);

	LOGIN_STRU login_s;
	memset(&login_s, 0, sizeof(LOGIN_STRU));
	strcpy(login_s.manufacturer_pwd, manufacturer_pwd);

	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::login), &login_s, true, 5000);
	fprintf(stderr, "%s:%s login result = %d\n", Time().c_str(), name.c_str(), result);
	LOGIN_RESP_STRU login_resp_s;
	result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::login_resp), (void *)&login_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for login response(%d) result = %d, manufacturer = %s\n", Time().c_str(), name.c_str(), result, login_resp_s.result, login_resp_s.manufacturer);
}
void LibAmtpcaTest::ModuleConfigTest ()
{
	static int module_num = 0;
	MODULE_CONF_STRU module_conf_s;
	memset(&module_conf_s, 0, sizeof(MODULE_CONF_STRU));
	module_conf_s.module_num = ++module_num;
	module_conf_s.module_type = AMTP_MODULE_TYPE_ENUM::NR;
	module_conf_s.chip_manufacturer = AMTP_CHIP_MANUFACTURER_ENUM::Qualcomm;
	strcpy(module_conf_s.chip_type, "Snapdragon 865");
	strcpy(module_conf_s.icd_version, "96-V3708-1");
	strcpy(module_conf_s.imei, "355672052457519");
	strcpy(module_conf_s.imsi, "460001357924680");
	strcpy(module_conf_s.module_system, "android");
	strcpy(module_conf_s.module_factory, "huawei");
	strcpy(module_conf_s.phone_number, "13488828654");
	strcpy(module_conf_s.network_operator, "CMCC");

	int result_s = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf), &module_conf_s, false, 5000);
	fprintf(stderr, "%s:%s module conf report result = %d\n", Time().c_str(), name.c_str(), result_s);
	MODULE_CONF_RESP_STRU module_conf_resp_s;
	int result_w = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp), (void *)&module_conf_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for module conf report response(%d) result = %d\n", Time().c_str(), name.c_str(), result_w, module_conf_resp_s.result);
}

void LibAmtpcaTest::ConfigTest ()
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	CONFIG_STRU config_s;
	memset(&config_s, 0, sizeof(CONFIG_STRU));
	//		config_s.version = 100;
	sprintf(config_s.atuid_version, "a1b2c3d4e5_123");

	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config), &config_s, true, 5000);
	fprintf(stderr, "%s:%s config result = %d\n", Time().c_str(), name.c_str(), result);

	CONFIG_RESP_STRU config_resp_s;
	result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config_resp), (void *)&config_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for config response(%d) atuid_version = %s, result = %d, update = %d, packet_count = %d, md5 = %s\n", Time().c_str(), name.c_str(), result, config_resp_s.atuid_version, config_resp_s.result, config_resp_s.update, config_resp_s.packet_count, config_resp_s.md5);
	sleep(3);
}
void LibAmtpcaTest::UploadFileRequestTest()
{
	UPLOAD_FILE_STRU upload_file_s;
	memset(&upload_file_s, 0, sizeof(UPLOAD_FILE_STRU));
	sprintf(upload_file_s.file_name, "9177700220190103104005ms1.lte");
	upload_file_s.retransmit = false;
	upload_file_s.module = 2;

	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file), &upload_file_s, true, 5000);
	fprintf(stderr, "%s:%s send upload file request(%d)\n", Time().c_str(), name.c_str(), result);

	UPLOAD_FILE_RESP_STRU upload_file_resp_s;
	result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp), (void *)&upload_file_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for upload file response(%d) result = %d\n", Time().c_str(), name.c_str(), result, upload_file_resp_s.result);
}
void LibAmtpcaTest::UploadFileDataTest()
{
	int max_packet_size = 1024 * 1024;
	FILE *dtlog_f = fopen("688k.txt", "rb");
	size_t file_size = 0;
	fseek(dtlog_f, 0L, SEEK_END);
	file_size = ftell(dtlog_f);

	int packet_count = ((int)file_size / max_packet_size) + (((int)(file_size % max_packet_size) == 0) ? 0 : 1);

	for (int packet_no = 1; packet_no <= packet_count; packet_no++)
	{
		//			SendFileData(x);
		int read_len = 0;
		if (packet_no == packet_count)
		{
			read_len = (int)file_size - (packet_no - 1) * max_packet_size;
		}
		else
		{
			read_len = max_packet_size;
		}

		fseek(dtlog_f, (packet_no - 1) * max_packet_size, SEEK_SET);
		unique_ptr<unsigned char[]> buf(new unsigned char[read_len]());
		fread(buf.get(), 1, read_len, dtlog_f);
		//			fclose(config_f);

		UPLOAD_FILE_DATA_STRU upload_file_data_s;
		memset(&upload_file_data_s, 0, sizeof(UPLOAD_FILE_DATA_STRU));
		upload_file_data_s.file_id = 123;
		upload_file_data_s.packet_no = packet_no;
		upload_file_data_s.data_len = read_len;
		upload_file_data_s.data = buf.get();

		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data), &upload_file_data_s, true, 5000);
		fprintf(stderr, "%s:%s Upload file data result = %d\n", Time().c_str(), name.c_str(), result);

		UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp), (void *)&upload_file_data_resp_s, 5000);
		fprintf(stderr, "%s:%s -----------------------------wait for upload file data response(%d) result = %d, file_id = %d, packet_no = %d\n", Time().c_str(), name.c_str(), result, upload_file_data_resp_s.result, upload_file_data_resp_s.file_id, upload_file_data_resp_s.packet_no);
		usleep(10 * 1000);
	}
	fclose(dtlog_f);
}
void LibAmtpcaTest::UploadFileEofTest()
{
	UPLOAD_EOF_STRU upload_eof_s;
	memset(&upload_eof_s, 0, sizeof(UPLOAD_EOF_STRU));
	sprintf(upload_eof_s.file_name, "9177700220190103104005ms1.lte");
	upload_eof_s.file_id = 12345;
	upload_eof_s.packet_count = 3;
	upload_eof_s.total_size = 67890;
	upload_eof_s.module = 3;
	sprintf(upload_eof_s.md5, "abcdefghijklmnopqrstuvwxyz98765");

	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof), &upload_eof_s, true, 5000);
	fprintf(stderr, "%s:%s send upload eof request(%d)\n", Time().c_str(), name.c_str(), result);

	UPLOAD_EOF_RESP_STRU upload_eof_resp_s;
	result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp), (void *)&upload_eof_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for upload eof response(%d) result = %d\n", Time().c_str(), name.c_str(), result, upload_eof_resp_s.result);
	sleep(3);
}
void LibAmtpcaTest::LogoutTest()
{
	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::logout), NULL, true, 5000);
	fprintf(stderr, "%s:%s logout result = %d\n", Time().c_str(), name.c_str(), result);
	LOGOUT_RESP_STRU logout_resp_s;
	result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::logout_resp), (void *)&logout_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for logout response(%d), result = %d\n", Time().c_str(), name.c_str(), result, logout_resp_s.result);
	sleep(3);
}
void LibAmtpcaTest::PerformanceTest()
{
	MODULE_CONF_STRU module_conf_s;
	memset(&module_conf_s, 0, sizeof(MODULE_CONF_STRU));
	module_conf_s.module_type = AMTP_MODULE_TYPE_ENUM::NR;
	module_conf_s.chip_manufacturer = AMTP_CHIP_MANUFACTURER_ENUM::Qualcomm;
	strcpy(module_conf_s.chip_type, "Snapdragon 865");
	strcpy(module_conf_s.icd_version, "96-V3708-1");
	strcpy(module_conf_s.imei, "355672052457519");
	strcpy(module_conf_s.phone_number, "13488828654");
	strcpy(module_conf_s.network_operator, "CMCC");

	struct timeval tv_s;
	char buf_s[32] = {0};
	char time_buf_s[64] = {0};
	struct timeval tv_e;
	char buf_e[32] = {0};
	char time_buf_e[64] = {0};

	gettimeofday(&tv_s, NULL);
	strftime(buf_s, sizeof(buf_s) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv_s.tv_sec));
	sprintf(time_buf_s, "%s.%06ld", buf_s, tv_s.tv_usec);

	int loop = 100000;
	int send_s_count = 0;
	int send_f_count = 0;
	int recv_s_count = 0;
	int recv_f_count = 0;
	while (loop--)
	{
		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf), &module_conf_s, false, 5000);
		if (result == 0)
		{
			send_s_count++;
		}
		else
		{
			send_f_count++;
		}

		MODULE_CONF_RESP_STRU module_conf_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp), (void *)&module_conf_resp_s, 5000);
		if (result == 0)
		{
			recv_s_count++;
		}
		else
		{
			recv_f_count++;
		}
	}
	gettimeofday(&tv_e, NULL);
	strftime(buf_e, sizeof(buf_e) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv_e.tv_sec));
	sprintf(time_buf_e, "%s.%06ld", buf_e, tv_e.tv_usec);

	fprintf(stderr, "%s:%s -----------------------------Performance test result\n", Time().c_str(), name.c_str());
	fprintf(stderr, "begin time: %s, %ld.%06ld sec\n", time_buf_s, tv_s.tv_sec, tv_s.tv_usec);
	fprintf(stderr, "end time: %s, %ld.%06ld sec\n", time_buf_e, tv_e.tv_sec, tv_e.tv_usec);
	int cost_time = (tv_e.tv_sec - tv_s.tv_sec) * 1000000 + tv_e.tv_usec - tv_s.tv_usec;
	fprintf(stderr, "cost time: %d ms\n", cost_time);
	fprintf(stderr, "send package: %d\n", send_s_count + send_f_count);
	fprintf(stderr, "send success: %d\n", send_s_count);
	fprintf(stderr, "send failed: %d\n", send_f_count);
	fprintf(stderr, "recv package: %d\n", recv_s_count + recv_f_count);
	fprintf(stderr, "recv success: %d\n", recv_s_count);
	fprintf(stderr, "recv failed: %d\n", recv_f_count);
	int send_app_bytes = (send_s_count + send_f_count) * 84;
	int recv_app_bytes = (recv_s_count + recv_f_count) * 17;
	fprintf(stderr, "send app bytes: %d\n", send_app_bytes);
	fprintf(stderr, "recv app bytes: %d\n", recv_app_bytes);
	fprintf(stderr, "send app capacity: %f Bps\n", (float)send_app_bytes / ((float)cost_time / 1000000.0));
	fprintf(stderr, "recv app capacity: %f Bps\n", (float)recv_app_bytes / ((float)cost_time / 1000000));
}
void LibAmtpcaTest::LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void * cmd_s, void* reserve)
{
	// fprintf(stderr, "%s:%s LibamtpcaCmdRecvCbfun recv cmd:0x%08X!\n", Time().c_str(), name.c_str(), static_cast<uint32_t>(cmd));
	if(cmd == AMTP_CMD_ENUM::login_resp)
	{
		LOGIN_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGIN_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv login response, manufacturer = %s, result = %d\n"
				, Time().c_str(), proc_id, resp_s.manufacturer, resp_s.result);
	}
	if(cmd == AMTP_CMD_ENUM::module_conf_resp)
	{
		MODULE_CONF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(MODULE_CONF_RESP_STRU));
		// fprintf(stderr, "%s:%s Callback recv module conf report response, result = %d\n", Time().c_str(), proc_id, resp_s.result);
	}
	if(cmd == AMTP_CMD_ENUM::config_resp)
	{
		CONFIG_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(CONFIG_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv config response, result = %d\n"
				, Time().c_str(), proc_id, resp_s.result);
	}
//	if(cmd == AMTP_CMD_ENUM::report_event_resp)
//	{
//		REPORT_EVENT_RESP_STRU resp_s;
//		memcpy(&resp_s, cmd_s, sizeof(REPORT_EVENT_RESP_STRU));
//		fprintf(stderr, "%s:%s Callback recv event response, result = %d\n"
//				, Time().c_str(), proc_id, resp_s.result);
//	}
//	if(cmd == AMTP_CMD_ENUM::alarm_resp)
//	{
//		ALARM_RESP_STRU resp_s;
//		memcpy(&resp_s, cmd_s, sizeof(ALARM_RESP_STRU));
//		fprintf(stderr, "%s:%s Callback recv alarm response, result = %d\n"
//				, Time().c_str(), proc_id, resp_s.result);
//	}
//	if(cmd == AMTP_CMD_ENUM::alarm_clear_resp)
//	{
//		ALARM_CLEAR_RESP_STRU resp_s;
//		memcpy(&resp_s, cmd_s, sizeof(ALARM_CLEAR_RESP_STRU));
//		fprintf(stderr, "%s:%s Callback recv alarm clear response, result = %d\n"
//				, Time().c_str(), proc_id, resp_s.result);
//	}
//	if(cmd == AMTP_CMD_ENUM::gps_info_resp)
//	{
//		GPS_INFO_RESP_STRU resp_s;
//		memcpy(&resp_s, cmd_s, sizeof(GPS_INFO_RESP_STRU));
//		fprintf(stderr, "%s:%s Callback recv GPS response, result = %d\n"
//				, Time().c_str(), proc_id, resp_s.result);
//	}
//	if(cmd == AMTP_CMD_ENUM::status_info_resp)
//	{
//		STATUS_INFO_RESP_STRU resp_s;
//		memcpy(&resp_s, cmd_s, sizeof(STATUS_INFO_RESP_STRU));
//		fprintf(stderr, "%s:%s Callback recv status response, result = %d\n"
//				, Time().c_str(), proc_id, resp_s.result);
//	}
	if(cmd == AMTP_CMD_ENUM::logout_resp)
	{
		LOGOUT_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGOUT_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv logout response, result = %d\n"
				, Time().c_str(), proc_id, resp_s.result);
	}
	if(cmd == AMTP_CMD_ENUM::config_data)
	{
		CONFIG_DATA_STRU *s = (CONFIG_DATA_STRU *)cmd_s;
		fprintf(stderr, "%s:%s Callback recv config data command, atuid_version = %s, packet_count = %d, packet_no = %d, data_length = %d\n"
				, Time().c_str(), proc_id, s->atuid_version, s->packet_count, s->packet_no, s->data_length);
		
		FILE *config_f;
		if(s->packet_no == 1)
		{
			config_f = fopen( "test_scheme.xml", "wb" );
		}
		else
		{
			config_f = fopen( "test_scheme.xml", "ab" );
		}
		fwrite(s->config_data, s->data_length, 1, config_f);
		fclose(config_f);
		
		CONFIG_DATA_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(CONFIG_DATA_RESP_STRU));
//		resp_s.version = s->version;
		memcpy(resp_s.atuid_version, s->atuid_version, sizeof(s->atuid_version));
		resp_s.packet_no = s->packet_no;
		resp_s.result = 0;
		
		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config_data_resp), &resp_s, false, 5000);
		fprintf(stderr, "%s:%s config data result = %d\n", Time().c_str(), name.c_str(), result);
		
	}
	if(cmd == AMTP_CMD_ENUM::config_notify)
	{
		CONFIG_NOTIFY_STRU *s = (CONFIG_NOTIFY_STRU *)cmd_s;
		fprintf(stderr, "%s:%s Callback recv config notify command, atuid_version = %s, packet_count = %d\n"
				, Time().c_str(), proc_id, s->atuid_version, s->packet_count);
		
		CONFIG_NOTIFY_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(CONFIG_NOTIFY_RESP_STRU));
		resp_s.result = 0;
		memcpy(resp_s.atuid_version, s->atuid_version, sizeof(s->atuid_version));

		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config_notify_resp), &resp_s, false, 5000);
		fprintf(stderr, "%s:%s config notify resp result = %d\n", Time().c_str(), name.c_str(), result);
	}
	if(cmd == AMTP_CMD_ENUM::restart_cmd)
	{
		RESTART_CMD_STRU *s = (RESTART_CMD_STRU *)cmd_s;
		fprintf(stderr, "%s:%s Callback recv restart command, ts_sec = %d, ts_usec = %d\n"
				, Time().c_str(), proc_id, s->ts_sec, s->ts_usec);
		
		RESTART_CMD_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(RESTART_CMD_RESP_STRU));
		resp_s.result = 0;

		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::restart_cmd_resp), &resp_s, false, 5000);
		fprintf(stderr, "%s:%s restart resp result = %d\n", Time().c_str(), name.c_str(), result);
	}
	if(cmd == AMTP_CMD_ENUM::upload_file_resp)
	{
		UPLOAD_FILE_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_FILE_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv upload file response, result = %d, file_name = %s, file_id = %d, file_size = %d, module = %d\n"
				, Time().c_str(), proc_id, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.file_size, resp_s.module);
	}
	if(cmd == AMTP_CMD_ENUM::upload_eof_resp)
	{
		UPLOAD_EOF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_EOF_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv upload eof response, result = %d, file_name = %s, file_id = %d, module = %d\n"
				, Time().c_str(), proc_id, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.module);
	}
	if(cmd == AMTP_CMD_ENUM::query_data)
	{
		QUERY_DATA_STRU *s = (QUERY_DATA_STRU *)cmd_s;
		fprintf(stderr, "%s:%s Callback recv query data, file_name = %s, file_id = %d, packet_no = %d\n"
				, Time().c_str(), proc_id, s->file_name, s->file_id, s->packet_no);
		
		QUERY_DATA_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(QUERY_DATA_RESP_STRU));
		resp_s.result = 0;

		int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::query_data_resp), &resp_s, false, 5000);
		fprintf(stderr, "%s:%s query data resp result = %d\n", Time().c_str(), name.c_str(), result);
	}
	if(cmd == AMTP_CMD_ENUM::upload_file_data_resp)
	{
		UPLOAD_FILE_DATA_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_FILE_DATA_RESP_STRU));
		fprintf(stderr, "%s:%s Callback recv upload file data response, result = %d, file_id = %d, packet_no = %d\n"
				, Time().c_str(), proc_id, resp_s.result, resp_s.file_id, resp_s.packet_no);
		
	}
}

int main(int argc, char * argv[])
{
	if(argc < 2)
	{
		PrintHelp(argv[0]);
		exit(0);
	}
	ParseArg(argc, argv);
	
	fprintf(stderr, "%s:******************************************libamtpa tester******************************************\n", Time().c_str());
	fprintf(stderr, "%s:The version of libamtpa tester is 1.0.0\n", Time().c_str());
	
	if(mode == 2 || mode == 4)
	{
		fprintf(stderr, "%s:%s begin!\n", Time().c_str(), proc_id);

		LibjwumqDealerClientTest test;
		test.Start(proc_id, mode);
	}
	else if(mode == 1 || mode == 3)
	{
		fprintf(stderr, "%s:%s begin!\n", Time().c_str(), proc_id);

		LibjwumqDealerServerTest test;
		test.Start(proc_id, mode);
	}
	else if(mode == 5)
	{
		fprintf(stderr, "%s:%s begin!\n", Time().c_str(), proc_id);

		LibAmtpsaTest test;
		test.Start(proc_id, mode);
	}
	else if(mode == 6)
	{
		fprintf(stderr, "%s:%s begin!\n", Time().c_str(), proc_id);

		LibAmtpcaTest test;
		test.Start(proc_id, mode);
	}
	fprintf(stderr, "%s:------------------------------------------libamtpa tester------------------------------------------\n", Time().c_str());
	return 0;
}

void PrintHelp(const char* prog)
{
	printf("%s usage.\n", prog);
	printf("-s: for test libamtpa mq model\n");
	
//	printf("eg:%s -model proc_id\n", prog);
//	printf("%s -p ptp_001\n", prog);
//	printf("%s -b pub_002\n", prog);
}

void ParseArg(int argc, char *argv[])
{
	int ch = 0;
	if (argc < 2)
	{
		if (!strcmp(argv[1], "?") ||
			!strcmp(argv[1], "--help") ||
			!strcmp(argv[1], "-h"))
		{
			PrintHelp(argv[0]);
			exit(0);
		}
	}
	strcpy(proc_id, argv[2]);
	
	while ((ch = getopt(argc, argv, "s:p:q:a:y:u:f:c:")) != -1)
	{
		switch (ch)
		{
			case 's':
				mode = 1;
				std::cout << "Hello jwumq dealer tester, begin server test!\n";
				break;
			case 'c':
				mode = 2;
				std::cout << "Hello jwumq dealer tester, begin client test!\n";
				break;
			case 'p':
				mode = 3;
				std::cout << "Hello amtp pb tester, begin server test!\n";
				break;
			case 'q':
				mode = 4;
				std::cout << "Hello amtp pb tester, begin client test!\n";
				break;
			case 'a':
				mode = 5;
				std::cout << "Hello amtp server access tester, begin server test!\n";
				break;
			case 'y':
				mode = 6;
				std::cout << "Hello amtp client access tester, begin client test!\n";
				break;
			case 'w':
				mode = 7;
				std::cout << "Hello, jmsg_tester, begin ptp web test!\n";
				break;
			case 'f':
				mode = 8;
				std::cout << "Hello, jmsg_tester, begin ptpf server test!\n";
				break;
			case '?':
			default:
				PrintHelp(argv[0]);
				exit(0);
				break;
		}
	}
}

string Time()
{
	struct timeval tv;
	char buf[32];
	char time_buf[64];
	 
	gettimeofday(&tv, NULL);
	strftime(buf, sizeof(buf)-1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
	sprintf(time_buf, "%s.%03d", buf, (int)(tv.tv_usec / 1000));
	return time_buf;
}

