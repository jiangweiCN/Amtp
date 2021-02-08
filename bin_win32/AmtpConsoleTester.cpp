// AmtpConsoleTester.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>  
#include <windows.h>
#include <vector>
#include <functional>
#include "../../source/libamtpa/libamtpa_define.h"

using namespace std;

string Time();
void PrintScreen(const char* lpcszFormat, ...);

using LIB_AMTPCA_VERSION = char* (*)();
using LIB_AMTPCA_INIT = int (*)(const char*, const char*, LIBAMTPCA_CMD_RECV_CBFUN);
using LIB_AMTPCA_SENDCMD = int (*)(uint32_t, void*, bool, uint32_t);
using LIB_AMTPSA_WAITFORCMD = int (*)(uint32_t, void*, int);
using LIB_AMTPCA_RELEASE = void (*)();

LIBAMTPCA_CMD_RECV_CBFUN LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve);
LIB_AMTPCA_SENDCMD lib_sendcmd_cb;

class LibAmtpcaTest
{
public:
	void Start();
private:
	void TestThread();
	
private:
	LIB_AMTPCA_VERSION lib_version;
	LIB_AMTPCA_INIT lib_init;
	LIB_AMTPCA_SENDCMD lib_sendcmd;
	LIB_AMTPSA_WAITFORCMD lib_waitforcmd;
	LIB_AMTPCA_RELEASE lib_release;

	string lib_name;
	string lic_name;
};

void LibAmtpcaTest::Start()
{
	thread t1(&LibAmtpcaTest::TestThread, this);
	t1.join();
}
void LibAmtpcaTest::TestThread()
{
	lib_name = "Amtpca.dll";
	PrintScreen("------------------Libamtpca Test(%s)------------------", lib_name.c_str());

	HMODULE hMod = LoadLibrary(lib_name.c_str());
	if (!hMod)
	{
		PrintScreen("Load %s failed!", lib_name.c_str());
		return;
	}
	lib_version = (LIB_AMTPCA_VERSION)GetProcAddress(hMod, "amtpca_version");
	lib_init = (LIB_AMTPCA_INIT)GetProcAddress(hMod, "amtpca_init");
	lib_sendcmd = (LIB_AMTPCA_SENDCMD)GetProcAddress(hMod, "amtpca_sendcmd");
	lib_waitforcmd = (LIB_AMTPSA_WAITFORCMD)GetProcAddress(hMod, "amtpca_waitforcmd");
	lib_release = (LIB_AMTPCA_RELEASE)GetProcAddress(hMod, "amtpca_release");

	lib_sendcmd_cb = lib_sendcmd;

	if (lib_version == NULL || lib_init == NULL || lib_sendcmd == NULL || lib_waitforcmd == NULL || lib_release == NULL)
	{
		PrintScreen("Get %s function error! %d, %d, %d, %d, %d, %s"
			, lib_name.c_str(), lib_version, lib_init, lib_sendcmd, lib_waitforcmd, lib_release, strerror(errno));
		return;
	}
	PrintScreen("%s version:%s", lib_name.c_str(), lib_version());

	lic_name = "dev.license";
	int result = lib_init(lic_name.c_str(), /*NULL*/"log", (LIBAMTPCA_CMD_RECV_CBFUN)LibamtpcaCmdRecvCbfun);
	PrintScreen("amtpca_init result:%d", result);
	if (result != 0)
	{
		return;
	}
	{
		LOGIN_STRU login_s;
		memset(&login_s, 0, sizeof(LOGIN_STRU));
		strcpy_s(login_s.manufacturer_pwd, "abcdefgh");

		PrintScreen("Login request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::login), &login_s, true, 5000);
		//PrintScreen("Login request result = %d", result);
		LOGIN_RESP_STRU login_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::login_resp), (void*)&login_resp_s, 5000);
		PrintScreen("-----------------wait for login response(%d) result = %d, manufacturer = %s"
			, result, login_resp_s.result, login_resp_s.manufacturer);
	}///LOGIN_STRU
	Sleep(1000);
	{
		MODULE_CONF_STRU module_conf_s;
		memset(&module_conf_s, 0, sizeof(MODULE_CONF_STRU));
		//		module_conf_s.channel_num = 1;
		module_conf_s.module_type = AMTP_MODULE_TYPE_ENUM::NR;
		module_conf_s.chip_manufacturer = AMTP_CHIP_MANUFACTURER_ENUM::Qualcomm;
		strcpy_s(module_conf_s.chip_type, "Snapdragon 865");
		strcpy_s(module_conf_s.icd_version, "96-V3708-1");
		strcpy_s(module_conf_s.imei, "355672052457519");
		strcpy_s(module_conf_s.phone_number, "13488828654");
		strcpy_s(module_conf_s.network_operator, "CMCC");
		PrintScreen("Module conf report request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf), &module_conf_s, true, 2000);
		//PrintScreen("Module conf report  requestresult = %d", result);
		MODULE_CONF_RESP_STRU module_conf_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp), (void*)&module_conf_resp_s, 5000);
		PrintScreen("-----------------wait for module conf report response(%d) result = %d", result, module_conf_resp_s.result);
	}///MODULE_CONF_STRU
	{
		UPLOAD_FILE_STRU upload_file_s;
		memset(&upload_file_s, 0, sizeof(UPLOAD_FILE_STRU));
		sprintf(upload_file_s.file_name, "9177700220190103104005ms1.lte");
		upload_file_s.retransmit = false;
		upload_file_s.module = 2;

		PrintScreen("Upload file request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file), &upload_file_s, true, 5000);
		//PrintScreen("Upload file request result = %d", result);

		UPLOAD_FILE_RESP_STRU upload_file_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp), (void*)&upload_file_resp_s, 5000);
		PrintScreen("-----------------wait for upload file response(%d) result = %d", result, upload_file_resp_s.result);
	}///UPLOAD_FILE_STRU
	{
		int max_packet_size = 102 * 1024;
		FILE* dtlog_f = fopen("688k.txt", "rb");
		size_t file_size = 0;
		fseek(dtlog_f, 0L, SEEK_END);
		file_size = ftell(dtlog_f);

		int packet_count = ((int)file_size / max_packet_size)
			+ (((int)(file_size % max_packet_size) == 0) ? 0 : 1);


		for (int packet_no = 1; packet_no <= packet_count; packet_no++)
		{
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

			UPLOAD_FILE_DATA_STRU upload_file_data_s;
			memset(&upload_file_data_s, 0, sizeof(UPLOAD_FILE_DATA_STRU));
			upload_file_data_s.file_id = 123;
			upload_file_data_s.packet_no = packet_no;
			upload_file_data_s.data_len = read_len;
			upload_file_data_s.data = buf.get();

			PrintScreen("Upload file data request %d", packet_no);
			int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data), &upload_file_data_s, true, 5000);

			UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_s;
			result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp), (void*)&upload_file_data_resp_s, 5000);
			PrintScreen("-----------------wait for upload file data response(%d) result = %d, file_id = %d, packet_no = %d"
				, result, upload_file_data_resp_s.result, upload_file_data_resp_s.file_id, upload_file_data_resp_s.packet_no);
			//Sleep(10);
		}
		fclose(dtlog_f);
	}///UPLOAD_FILE_DATA_STRU
	{
		UPLOAD_EOF_STRU upload_eof_s;
		memset(&upload_eof_s, 0, sizeof(UPLOAD_EOF_STRU));
		sprintf(upload_eof_s.file_name, "9177700220190103104005ms1.lte");
		upload_eof_s.file_id = 12345;
		upload_eof_s.packet_count = 3;
		upload_eof_s.total_size = 67890;
		upload_eof_s.module = 3;
		sprintf(upload_eof_s.md5, "abcdefghijklmnopqrstuvwxyz98765");

		PrintScreen("Upload eof request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof), &upload_eof_s, true, 5000);

		UPLOAD_EOF_RESP_STRU upload_eof_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp), (void*)&upload_eof_resp_s, 5000);
		PrintScreen("-----------------wait for upload eof response(%d) result = %d"
			, result, upload_eof_resp_s.result);
	}///UPLOAD_EOF_STRU
	{
		PrintScreen("Logout request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::logout), NULL, true, 5000);
		LOGOUT_RESP_STRU logout_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::logout_resp), (void*)&logout_resp_s, 5000);
		PrintScreen("-----------------wait for logout response(%d) result = %d", result, logout_resp_s.result);
	}///LOGOUT_
	Sleep(2000);
	{
		CONFIG_STRU config_s;
		memset(&config_s, 0, sizeof(CONFIG_STRU));
		sprintf_s(config_s.atuid_version, "a1b2c3d4e5_123");

		PrintScreen("Config report request");
		result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config), &config_s, true, 5000);
		//PrintScreen("Config report  requestresult = %d", result);
		CONFIG_RESP_STRU config_resp_s;
		result = lib_waitforcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config_resp), (void*)&config_resp_s, 5000);
		PrintScreen("-----------------wait for config response(%d) atuid_version = %s, result = %d, update = %d, packet_count = %d, md5 = %s"
			, result, config_resp_s.atuid_version, config_resp_s.result
			, config_resp_s.update, config_resp_s.packet_count, config_resp_s.md5);

	}///CONFIG_STRU
	

	Sleep(5000);
	lib_release();

	FreeLibrary(hMod);
}
LIBAMTPCA_CMD_RECV_CBFUN LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve)
{
	//PrintScreen("LibamtpcaCmdRecvCbfun recv cmd:0x%08X!", static_cast<uint32_t>(cmd));
	if (cmd == AMTP_CMD_ENUM::login_resp)
	{
		LOGIN_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGIN_RESP_STRU));
		PrintScreen("Callback recv login response, manufacturer = %s, result = %d", resp_s.manufacturer, resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::module_conf_resp)
	{
		MODULE_CONF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(MODULE_CONF_RESP_STRU));
		PrintScreen("Callback recv module conf report response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::config_resp)
	{
		CONFIG_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(CONFIG_RESP_STRU));
		PrintScreen("Callback recv config response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::logout_resp)
	{
		LOGOUT_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGOUT_RESP_STRU));
		PrintScreen("Callback recv logout response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::config_data)
	{
		CONFIG_DATA_STRU* s = (CONFIG_DATA_STRU*)cmd_s;
		PrintScreen("Callback recv config data command, atuid_version = %s, packet_count = %d, packet_no = %d, data_length = %d"
			, s->atuid_version, s->packet_count, s->packet_no, s->data_length);

		FILE* config_f;
		if (s->packet_no == 1)
		{
			config_f = fopen("test_scheme.xml", "wb");
		}
		else
		{
			config_f = fopen("test_scheme.xml", "ab");
		}
		fwrite(s->config_data, s->data_length, 1, config_f);
		fclose(config_f);

		CONFIG_DATA_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(CONFIG_DATA_RESP_STRU));
		//		resp_s.version = s->version;
		memcpy(resp_s.atuid_version, s->atuid_version, sizeof(s->atuid_version));
		resp_s.packet_no = s->packet_no;
		resp_s.result = 0;

		int result = lib_sendcmd_cb(static_cast<uint32_t>(AMTP_CMD_ENUM::config_data_resp), &resp_s, false, 5000);
		PrintScreen("config data result = %d", result);

	}
	if (cmd == AMTP_CMD_ENUM::config_notify)
	{
		CONFIG_NOTIFY_STRU* s = (CONFIG_NOTIFY_STRU*)cmd_s;
		PrintScreen("Callback recv config notify command, atuid_version = %s, packet_count = %d"
			, s->atuid_version, s->packet_count);

		CONFIG_NOTIFY_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(CONFIG_NOTIFY_RESP_STRU));
		resp_s.result = 0;
		memcpy(resp_s.atuid_version, s->atuid_version, sizeof(s->atuid_version));

		int result = lib_sendcmd_cb(static_cast<uint32_t>(AMTP_CMD_ENUM::config_notify_resp), &resp_s, false, 5000);
		PrintScreen("Config notify resp result = %d", result);
	}
	if (cmd == AMTP_CMD_ENUM::upload_file_resp)
	{
		UPLOAD_FILE_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_FILE_RESP_STRU));
		PrintScreen("Callback recv upload file response, result = %d, file_name = %s, file_id = %d, file_size = %d, module = %d"
			, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.file_size, resp_s.module);
	}
	if (cmd == AMTP_CMD_ENUM::upload_eof_resp)
	{
		UPLOAD_EOF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_EOF_RESP_STRU));
		PrintScreen("Callback recv upload eof response, result = %d, file_name = %s, file_id = %d, module = %d"
			, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.module);
	}
	//if (cmd == AMTP_CMD_ENUM::query_data)
	//{
	//	QUERY_DATA_STRU* s = (QUERY_DATA_STRU*)cmd_s;
	//	fprintf(stderr, "%s:%s Callback recv query data, file_name = %s, file_id = %d, packet_no = %d\n"
	//		, Time().c_str(), proc_id, s->file_name, s->file_id, s->packet_no);

	//	QUERY_DATA_RESP_STRU resp_s;
	//	memset(&resp_s, 0, sizeof(QUERY_DATA_RESP_STRU));
	//	resp_s.result = 0;

	//	int result = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::query_data_resp), &resp_s, false, 5000);
	//	fprintf(stderr, "%s:%s query data resp result = %d\n", Time().c_str(), name.c_str(), result);
	//}
	if (cmd == AMTP_CMD_ENUM::upload_file_data_resp)
	{
		UPLOAD_FILE_DATA_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_FILE_DATA_RESP_STRU));
		PrintScreen("Callback recv upload file data response, result = %d, file_id = %d, packet_no = %d"
			, resp_s.result, resp_s.file_id, resp_s.packet_no);
	}
	return 0;
}



int main()
{
	std::cout << "Hello Amtp!\n";
	LibAmtpcaTest test;
	test.Start();
	system("pause");
}
string Time()
{
	char buf[64];

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms

	//gettimeofday(&tv, NULL);
	sprintf_s(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d.%03d"
		, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}
void PrintScreen(const char* lpcszFormat, ...)
{
	string strResult;
	if (NULL != lpcszFormat)
	{
		va_list marker = NULL;
		va_start(marker, lpcszFormat); //初始化变量参数
		size_t nLength = _vscprintf(lpcszFormat, marker) + 1; //获取格式化字符串长度
		std::vector<char> vBuffer(nLength, '\0'); //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker); //重置变量参数
	}
	fprintf(stderr, "%s---%s\n", Time().c_str(), strResult.c_str());
}