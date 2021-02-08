//
//  libamtpa_define.h
//  Amtp
//
//  Created by jiangwei on 2020/1/7.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libamtpa_define_h
#define libamtpa_define_h

#include <functional>

#define LIB_AMTPA_SUCCESS			0
#define LIB_AMTPA_IS_WORKING		-101
#define LIB_AMTPA_NO_WORKING		-102
#define LIB_AMTPA_PARA_ERROR		-103
#define LIB_AMTPA_OPEN_LIB_FAILED	-104
#define LIB_AMTPA_VERSION_FAILED	-105
#define LIB_AMTPA_TIMEOUT			-106
#define LIB_AMTPA_QUEUE_EMPTY		-107
#define LIB_AMTPA_MSG_IS_NULL		-108
#define LIB_AMTPA_MSG_LEN_ERROR		-109
#define LIB_AMTPA_SEND_PARA_ERROR	-110
#define LIB_AMTPA_LICENSE_ERROR		-111
#define LIB_AMTPA_MQ_SETUP_ERROR	-112
#define LIB_AMTPA_UNKNOWN_MSG		-113
#define LIB_AMTPA_SEND_ERROR		-114
#define LIB_AMTPA_WAITFORCMD_ERROR	-115
#define LIB_AMTPA_NO_INIT			-116


using byte = unsigned char;
constexpr auto FILENAME_LENGTH = 64;

enum class AMTP_CMD_ENUM : uint32_t
{
	reserve					= 0x00,
	login					= 0x00010001,
	login_resp				= 0x00020001,
	logout					= 0x00030001,
	logout_resp				= 0x00040001,
	config					= 0x00050001,
	config_resp				= 0x00060001,
	report_event			= 0x00070001,
	report_event_resp		= 0x00080001,
	alarm					= 0x00090001,
	alarm_resp				= 0x000A0001,
	alarm_clear				= 0x000B0001,
	alarm_clear_resp		= 0x000C0001,
	gps_info				= 0x000D0001,
	gps_info_resp			= 0x000E0001,
	status_info				= 0x000F0001,
	status_info_resp		= 0x00100001,
	config_data				= 0x00110001,
	config_data_resp		= 0x00120001,
	config_notify			= 0x00130001,
	config_notify_resp		= 0x00140001,
	upload_file				= 0x00150001,
	upload_file_resp		= 0x00160001,
	upload_eof				= 0x00170001,
	upload_eof_resp			= 0x00180001,
	restart_cmd				= 0x00190001,
	restart_cmd_resp		= 0x001A0001,
	query_data				= 0x001B0001,
	query_data_resp			= 0x001C0001,
	upload_file_data		= 0x001D0001,
	upload_file_data_resp	= 0x001E0001,
	module_conf				= 0x001F0001,
	module_conf_resp		= 0x00200001,
};

enum class AMTP_MODULE_TYPE_ENUM : uint32_t
{
	unknown		= 0,
	GPS			= 1,
	LTE			= 2,
	NR			= 3,
	SCANNER		= 4,
	WLAN		= 5,
	NB			= 6,
	other		= 255,
};

enum class AMTP_CHIP_MANUFACTURER_ENUM : uint32_t
{
	Qualcomm	= 0,
	Hisilicon	= 1,
	other		= 255,
};

#if defined(_WIN32)
using LIBAMTPCA_CMD_RECV_CBFUN = void(*)(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve);
#else
using LIBAMTPCA_CMD_RECV_CBFUN = std::function<void(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve)>;
#endif

using LOGIN_STRU = struct login_stru
{
//	char		box_id[32];
//	uint32_t	conf_version;
	char		manufacturer_pwd[32];
};
using LOGIN_RESP_STRU = struct login_resp_stru
{
//	uint32_t		conf_version;
	char		manufacturer[32];
	int32_t		result;
};

using MODULE_CONF_STRU = struct module_conf_stru
{
//	uint32_t						channel_num;
	AMTP_MODULE_TYPE_ENUM			module_type;
	AMTP_CHIP_MANUFACTURER_ENUM		chip_manufacturer;
	char							chip_type[32];
	char							icd_version[32];
	char							imei[32];
	char							phone_number[16];
	char							network_operator[16];
};
using MODULE_CONF_RESP_STRU = struct module_conf_resp_stru
{
	int32_t		result;
};

using LOGOUT_RESP_STRU = struct logout_resp_stru
{
	int32_t		result;
};

using CONFIG_STRU = struct config_stru
{
	char		atuid_version[32];
};

using CONFIG_RESP_STRU = struct config_resp_stru
{
	char		atuid_version[32];
	int32_t		result;
	bool		update;//true:need to update
	int32_t		packet_count;//if update = false packet = 0;else .....
	char		md5[32];//if update = false md5 is 0;else .....
};

using CONFIG_DATA_STRU = struct config_data_stru
{
	char		atuid_version[32];
	int32_t		packet_count;
	int32_t		packet_no;
	uint32_t	data_length;
	char		*config_data;
};

using CONFIG_DATA_RESP_STRU = struct config_data_resp_stru
{
	char		atuid_version[32];
	int32_t		packet_no;
	int32_t		result;
};

using CONFIG_NOTIFY_STRU = struct config_notify_stru
{
	char		atuid_version[32];
	int32_t		packet_count;
	char		md5[32];
};

using CONFIG_NOTIFY_RESP_STRU = struct config_notify_resp_stru
{
	char		atuid_version[32];
	int32_t		result;
};

using REPORT_EVENT_STRU = struct report_event_stru
{
	uint32_t	ts_sec;
	uint32_t	ts_usec;
	uint32_t	module;
	uint32_t	event_code;
};

using REPORT_EVENT_RESP_STRU = struct report_event_resp_stru
{
	int32_t		result;
};

using ALARM_STRU = struct alarm_stru
{
	uint32_t	module;
	uint32_t	seq;
	uint32_t	alarm_code;
	uint32_t	reason_code;
	uint32_t	ts_sec;
	uint32_t	ts_usec;
};

using ALARM_RESP_STRU = struct alarm_resp_stru
{
	int32_t		result;
};

using ALARM_CLEAR_STRU = struct alarm_clear_stru
{
	uint32_t	module;
	uint32_t	seq;
	uint32_t	alarm_code;
	uint32_t	ts_sec;
	uint32_t	ts_usec;
};

using ALARM_CLEAR_RESP_STRU = struct alarm_clear_resp_stru
{
	int32_t		result;
};

using GPS_INFO_STRU = struct gps_info_stru
{
	float		longitude;
	float		latitude;
	float		altitude;
	float		speed;
	float		heading;
	uint32_t	ts_sec;
	uint32_t	ts_usec;
};

using GPS_INFO_RESP_STRU = struct gps_info_resp_stru
{
	int32_t		result;
};

using STATUS_INFO_STRU = struct status_info_stru
{
	uint32_t	power_mode;
	uint32_t	files_letf;
	float		temperature;
	float		disk_space;
	uint32_t	ts_sec;
	uint32_t	ts_usec;
};

using STATUS_INFO_RESP_STRU = struct status_info_resp_stru
{
	int32_t		result;
};

using UPLOAD_FILE_STRU = struct upload_file_stru
{
	char		file_name[FILENAME_LENGTH];
	bool		retransmit;
	uint32_t	module;
};

using UPLOAD_FILE_RESP_STRU = struct upload_file_resp_stru
{
	char		file_name[FILENAME_LENGTH];
	uint32_t	file_id;
	uint32_t	file_size;
	uint32_t	module;
	int32_t		result;
};

using UPLOAD_FILE_DATA_STRU = struct upload_file_data_stru
{
	uint32_t		file_id;
	uint32_t		packet_no;
	uint32_t		data_len;
	unsigned char *	data;
};

using UPLOAD_FILE_DATA_RESP_STRU = struct upload_file_data_resp_stru
{
	uint32_t	file_id;
	uint32_t	packet_no;
	int32_t		result;
};

using UPLOAD_EOF_STRU = struct upload_eof_stru
{
	char		file_name[FILENAME_LENGTH];
	uint32_t	file_id;
	uint32_t	packet_count;
	uint32_t	total_size;
	uint32_t	module;
	char		md5[32];
};

using UPLOAD_EOF_RESP_STRU = struct upload_eof_resp_stru
{
	char		file_name[FILENAME_LENGTH];
	uint32_t	file_id;
	uint32_t	packet_count;
	uint32_t	module;
	int32_t		result;
};

using QUERY_DATA_STRU = struct query_data_stru
{
	char		file_name[FILENAME_LENGTH];
	uint32_t	file_id;
	uint32_t	packet_no;
};

using QUERY_DATA_RESP_STRU = struct query_data_resp_stru
{
	int32_t		result;
};

using RESTART_CMD_STRU = struct restart_cmd_stru
{
	uint32_t	ts_sec;
	uint32_t	ts_usec;
};

using RESTART_CMD_RESP_STRU = struct restart_cmd_resp_stru
{
	int32_t		result;
};



#endif /* libamtpa_define_h */
