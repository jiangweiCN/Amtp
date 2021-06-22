//
//  amtpca.hpp
//  Amtp
//
//  Created by jiangwei on 2020/2/27.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtpca_hpp
#define amtpca_hpp

#include <stdio.h>
#include <condition_variable>
#include <memory>
#include <map>
#include <thread>
// #include <queue>
// #include <mutex>
#include "amtpca_define.h"
#include "../libamtpa_define.h"
#include "../../jwumq_external/jwumq_define.h"
#include "../../license/libamtpa_license.hpp"
#include "../../jwumq_external/jwumq_message.hpp"
#include "msg_event_objects.hpp"
/////////#include "sys_info/system_info.hpp"
#if defined(_WIN32)
#include "zmq.h"
#else
#include "/home/libdev/zmq/include/zmq.h"
#endif

using namespace std;

#if defined(_WIN32)

#else
using LIB_JWUMQ_VERSION = char *(*)();
using LIB_JWUMQ_DEALER_LOAD = void *(*)();
using LIB_JWUMQ_SETUP = int (*)(void *, JWUMQ_SETUP_CONF_T *, JWUMQ_RECV_CALLBACK);
using LIB_JWUMQ_SEND = int (*)(void *, void *);
using LIB_JWUMQ_RELEASE = void (*)(void *);
#endif

class Amtpca
{
public:
	Amtpca();
	~Amtpca(void);

	int Init(const char *license_path, const char *log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back);
	void Release();
	int SendCmd(uint32_t cmd, void *cmd_s, bool sync, uint32_t timeout);
	int WaitForCmd(int handle, uint32_t cmd, void *msg_stru, uint32_t timeout);

private:
	void DataHandlerThread();
	void CmdHandlerThread();

	int RecvCmdCallback(void *msg);
	int RecvDataCallback(void *msg);
	///////int InitLog(const char * log_path);
	///////void ReleaseLog();
	int SendLogin(LOGIN_STRU *s, bool sync, uint32_t timeout);
	int SendLogout(bool sync, uint32_t timeout);
	int SendModuleConf(MODULE_CONF_STRU *s, bool sync, uint32_t timeout);
	int SendConfig(CONFIG_STRU *s, bool sync, uint32_t timeout);
	int SendConfigDataResp(CONFIG_DATA_RESP_STRU *s, bool sync, uint32_t timeout);
	int SendConfigNotifyResp(CONFIG_NOTIFY_RESP_STRU *s, bool sync, uint32_t timeout);
	int SendUploadFile(UPLOAD_FILE_STRU *s, bool sync, uint32_t timeout);
	int SendUploadEof(UPLOAD_EOF_STRU *s, bool sync, uint32_t timeout);
	int SendQueryDataResp(QUERY_DATA_RESP_STRU *s, bool sync, uint32_t timeout);
	int SendUploadFileData(UPLOAD_FILE_DATA_STRU *s, bool sync, uint32_t timeout);
	void RecvLoginResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvLogoutResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvModuleConfResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvConfigResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvConfigData(JwumqMessage *);
	void RecvConfigNotify(JwumqMessage *);
	void RecvUploadFileResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvUploadEofResp(JwumqMessage *, EVENT_OBJECT_S *);
	void RecvQueryData(JwumqMessage *);
	void RecvUploadFileDataResp(JwumqMessage *, EVENT_OBJECT_S *);

	int WaitForAck(uint32_t handle, int timeout);
	void SetupInprocMq(const char *mq_id);

	////////	SYS_INFO_T GetSysInfo();
public:
private:
	void *lib_handle;
	void *cmd_handle;
	void *data_handle;
	string cmd_mq_id;
	string data_mq_id;

	thread data_handler_thread;
	thread cmd_handler_thread;
	volatile int thread_loop;

	/*LIB_JWUMQ_VERSION lib_version;
	LIB_JWUMQ_DEALER_LOAD lib_jwumq_dealer_load;
	LIB_JWUMQ_SETUP lib_jwumq_setup;
	LIB_JWUMQ_SEND lib_jwumq_send;
	LIB_JWUMQ_RELEASE lib_jwumq_release;*/
	//////////////////////////////////////////For cross platform redefinition

#if defined(_WIN32)

#else
	LIB_JWUMQ_VERSION jwumq_version;
	LIB_JWUMQ_DEALER_LOAD jwumq_dealer_load;
	LIB_JWUMQ_SETUP jwumq_setup;
	LIB_JWUMQ_SEND jwumq_send;
	LIB_JWUMQ_RELEASE jwumq_release;

#endif

	bool blog;
	LIBAMTPCA_CMD_RECV_CBFUN cmd_recv_callback;

	LIC_CONF_T amtp_conf;
	AmtpaLicense amtp_license;
	uint16_t cmd_sn;
	unsigned char token[TOKEN_BUFFER_SIZE];

	mutex send_mutex;

	unique_ptr<msg_event_objects> native_event_objects;
	unique_ptr<msg_event_objects> ack_objects;

	void *inproc_context;
	void *send_socket;
	void *recv_socket;
	char inproc_send_id[MAX_IDENTITY_BUF_SIZE];
	char inproc_recv_id[MAX_IDENTITY_BUF_SIZE];
	char inproc_address[MAX_ADDRESS_BUF_SIZE];
};
#endif /* amtpca_hpp */
