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
#include "amtpca_define.h"
#include "../libamtpa_define.h"
#include "../../jwumq/jwumq_define.h"
#include "../../license/libamtpa_license.hpp"
#include "../../jwumq/jwumq_message.hpp"
/////////#include "sys_info/system_info.hpp"

using namespace std;

#if defined(_WIN32)

#else
using LIB_JWUMQ_VERSION = char* (*)();
using LIB_JWUMQ_DEALER_LOAD = void* (*)();
using LIB_JWUMQ_SETUP = int (*)(void*, JWUMQ_SETUP_CONF_T*, JWUMQ_RECV_CALLBACK);
using LIB_JWUMQ_SEND = int (*)(void*, void*);
using LIB_JWUMQ_RELEASE = void (*)(void*);
#endif

using RECV_MUTEX_MAP = map <AMTP_CMD_ENUM, unique_ptr<mutex>>;
using RECV_CV_MAP = map <AMTP_CMD_ENUM, unique_ptr<condition_variable>>;

class Amtpca
{
public:
	Amtpca();
	~Amtpca(void);
	
	int Init(const char * license_path, const char * log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back);
	void Release();
	int SendCmd(uint32_t cmd, void * cmd_s, bool sync, uint32_t timeout);
	int WaitForCmd(uint32_t cmd, void * s, uint32_t timeout);

private:
	void AliveThread();

	int RecvCmdCallback(void * msg);
	int RecvDataCallback(void * msg);
	///////int InitLog(const char * log_path);
	///////void ReleaseLog();
	int SendLogin(LOGIN_STRU * s, bool sync, uint32_t timeout);
	int SendLogout(bool sync, uint32_t timeout);
	int SendModuleConf(MODULE_CONF_STRU * s, bool sync, uint32_t timeout);
	int SendEvent(REPORT_EVENT_STRU * s, bool sync, uint32_t timeout);
	int SendAlarm(ALARM_STRU * s, bool sync, uint32_t timeout);
	int SendAlarmClear(ALARM_CLEAR_STRU * s, bool sync, uint32_t timeout);
	int SendConfig(CONFIG_STRU * s, bool sync, uint32_t timeout);
	int SendGpsInfo(GPS_INFO_STRU * s, bool sync, uint32_t timeout);
	int SendStatusInfo(STATUS_INFO_STRU * s, bool sync, uint32_t timeout);
	int SendConfigDataResp(CONFIG_DATA_RESP_STRU * s, bool sync, uint32_t timeout);
	int SendConfigNotifyResp(CONFIG_NOTIFY_RESP_STRU * s, bool sync, uint32_t timeout);
	int SendRestartCmdResp(RESTART_CMD_RESP_STRU * s, bool sync, uint32_t timeout);
	int SendUploadFile(UPLOAD_FILE_STRU * s, bool sync, uint32_t timeout);
	int SendUploadEof(UPLOAD_EOF_STRU * s, bool sync, uint32_t timeout);
	int SendQueryDataResp(QUERY_DATA_RESP_STRU * s, bool sync, uint32_t timeout);
	int SendUploadFileData(UPLOAD_FILE_DATA_STRU * s, bool sync, uint32_t timeout);
	void RecvLoginResp(JwumqMessage * );
	void RecvLogoutResp(JwumqMessage * );
	void RecvModuleConfResp(JwumqMessage * );
	void RecvEventResp(JwumqMessage * );
	void RecvAlarmResp(JwumqMessage * );
	void RecvAlarmClearResp(JwumqMessage * );
	void RecvConfigResp(JwumqMessage * );
	void RecvGpsInfoResp(JwumqMessage * );
	void RecvStatusInfoResp(JwumqMessage * );
	void RecvConfigData(JwumqMessage * );
	void RecvConfigNotify(JwumqMessage * );
	void RecvRestartCmd(JwumqMessage * );
	void RecvUploadFileResp(JwumqMessage * );
	void RecvUploadEofResp(JwumqMessage * );
	void RecvQueryData(JwumqMessage * );
	void RecvUploadFileDataResp(JwumqMessage * );
	
////////	SYS_INFO_T GetSysInfo();
public:
	
private:
	void *lib_handle;
	void * cmd_handle;
	void * data_handle;
	string cmd_mq_id;
	string data_mq_id;

	thread alive_thread;
	int alive_thread_loop;
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
	uint32_t cmd_sn;
	uint32_t data_sn;
	unsigned char token[TOKEN_BUFFER_SIZE];
	
	mutex send_mutex;
	mutex ack_mutex;
	condition_variable ack_cv;
	
	mutex wait_mutex;
	LOGIN_RESP_STRU login_resp_tmp_s;
	LOGOUT_RESP_STRU logout_resp_tmp_s;
	MODULE_CONF_RESP_STRU module_conf_resp_tmp_s;
	REPORT_EVENT_RESP_STRU report_event_resp_tmp_s;
	ALARM_RESP_STRU alarm_resp_tmp_s;
	ALARM_CLEAR_RESP_STRU alarm_clear_resp_tmp_s;
	CONFIG_RESP_STRU config_resp_tmp_s;
	GPS_INFO_RESP_STRU gps_info_resp_tmp_s;
	STATUS_INFO_RESP_STRU status_info_resp_tmp_s;
	UPLOAD_FILE_RESP_STRU upload_file_resp_tmp_s;
	UPLOAD_EOF_RESP_STRU upload_eof_resp_tmp_s;
	UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_tmp_s;
	
	
	RECV_MUTEX_MAP recv_mutex_map;
	RECV_CV_MAP recv_cv_map;
	
};
#endif /* amtpca_hpp */
