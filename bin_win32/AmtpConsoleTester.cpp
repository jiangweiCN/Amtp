// AmtpConsoleTester.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>  
#include <windows.h>
#include <vector>
#include <io.h>
#include <functional>
#include <fstream>
#include "../../source/libamtpa/libamtpa_define.h"

using namespace std;

string Time();
void PrintScreen(const char* lpcszFormat, ...);

using LIB_AMTPCA_VERSION = char* (*)();
using LIB_AMTPCA_INIT = int (*)(const char*, const char*, LIBAMTPCA_CMD_RECV_CBFUN);
using LIB_AMTPCA_SENDCMD = int (*)(uint32_t, void*, bool, uint32_t);
using LIB_AMTPCA_WAITFORCMD = int (*)(int, uint32_t, void*, int);
using LIB_AMTPCA_RELEASE = void (*)();

LIBAMTPCA_CMD_RECV_CBFUN LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve);
LIB_AMTPCA_SENDCMD lib_sendcmd_cb;

class LibAmtpcaTest
{
public:
	void Start();
private:
	void TestThread(); 
	void LoginTest();
	void ModuleConfigTest();
	void ConfigTest();
	void UploadFileRequestTest();
	void UploadFileDataTest();
	void UploadFileEofTest();
	void LogoutTest();
	void MultithreadingUploadFile();
	void MultithreadingUploadFileThread(int thread_index, int packet_size);
	
private:
	LIB_AMTPCA_VERSION lib_version;
	LIB_AMTPCA_INIT lib_init;
	LIB_AMTPCA_SENDCMD lib_sendcmd;
	LIB_AMTPCA_WAITFORCMD lib_waitforcmd;
	LIB_AMTPCA_RELEASE lib_release;

	string lib_name;
	string lic_name;
};

void LibAmtpcaTest::Start()
{
	thread t1(&LibAmtpcaTest::TestThread, this);
	t1.join();
}

void getFileNames(string path, vector<string>& filenames)
{
	//文件句柄，win10用long long，win7用long就可以了
	INT64 hFile = 0;
	//文件信息 
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之 //如果不是,加入列表 
			if ((fileinfo.attrib & _A_SUBDIR))
			{

			}
			else
			{
				//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				filenames.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
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
	lib_waitforcmd = (LIB_AMTPCA_WAITFORCMD)GetProcAddress(hMod, "amtpca_waitforcmd");
	lib_release = (LIB_AMTPCA_RELEASE)GetProcAddress(hMod, "amtpca_release");

	lib_sendcmd_cb = lib_sendcmd;

	if (lib_version == NULL || lib_init == NULL || lib_sendcmd == NULL || lib_waitforcmd == NULL || lib_release == NULL)
	{
		PrintScreen("Get %s function error! %d, %d, %d, %d, %d, %s"
			, lib_name.c_str(), lib_version, lib_init, lib_sendcmd, lib_waitforcmd, lib_release, strerror(errno));
		return;
	}
	PrintScreen("%s version:%s", lib_name.c_str(), lib_version());

	

	while (1)
	{
		char choice[64] = { 0 };
		printf("=======================================================================\n");
		printf("please input your choice\n");
		printf("1: Init\n");
		printf("2: Login\n");
		printf("3: Module conf report\n");
		printf("4: Config request\n");
		printf("5: Upload file request\n");
		printf("6: Upload file data\n");
		printf("7: Upload file eof\n");
		printf("8: Logout\n");
		printf("9: multithreading Upload file\n");
		printf("10: Performance test\n");
		printf("0: Exit test\n");
		printf("Select:");
		scanf("%s", choice);

		int select = atoi(choice);

		if (select == 1)
		{
			ofstream File;
			File.open("file_.txt");
			File.close();
			string path = "lic";
			vector<string> lic_files;
			getFileNames(path, lic_files);
			int file_numbers = (int)lic_files.size();

			fprintf(stderr, "\nlicense file list:\n");
			for (int i = 0; i < file_numbers; ++i)
			{
				fprintf(stderr, "%d:%s\n", i, lic_files[i].c_str());
			}
			int file_index = -1;
			printf("please input license file index: ");
			scanf("%d", &file_index);

			string lic_file = path + "\\" + lic_files[file_index];
			
			int result = lib_init(lic_file.c_str(), /*NULL*/"log", (LIBAMTPCA_CMD_RECV_CBFUN)LibamtpcaCmdRecvCbfun);
			PrintScreen("amtpca_init result:%d", result);
			if (result != 0)
			{
				return;
			}
		}
		else if (select == 2)
		{
			LoginTest();
		}
		else if (select == 3)
		{
			ModuleConfigTest();
		}
		else if (select == 4)
		{
			ConfigTest();
		}
		else if (select == 5)
		{
			UploadFileRequestTest();
		}
		else if (select == 6)
		{
			UploadFileDataTest();
		}
		else if (select == 7)
		{
			UploadFileEofTest();
		}
		else if (select == 8)
		{
			LogoutTest();
		}
		else if (select == 9)
		{
			MultithreadingUploadFile();
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
	
	//Sleep(5000);
	lib_release();

	FreeLibrary(hMod);
}
void LibAmtpcaTest::LoginTest()
{
	char manufacturer_pwd[64] = { 0 };
	printf("please input manufacturer pwd: ");
	scanf("%s", manufacturer_pwd);

	LOGIN_STRU login_s;
	memset(&login_s, 0, sizeof(LOGIN_STRU));
	strcpy(login_s.manufacturer_pwd, manufacturer_pwd);

	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::login), &login_s, true, 5000);
	PrintScreen("Login request handle = %d", handle);
	LOGIN_RESP_STRU login_resp_s;
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::login_resp), (void*)&login_resp_s, 5000);
	PrintScreen("-----------------wait for login response(%d) result = %d, manufacturer = %s"
		, result, login_resp_s.result, login_resp_s.manufacturer);
}
void LibAmtpcaTest::ModuleConfigTest()
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

	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf), &module_conf_s, false, 5000);
	PrintScreen("Module conf report  handle = %d", handle);
	MODULE_CONF_RESP_STRU module_conf_resp_s;
	int result_w = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::module_conf_resp), (void*)&module_conf_resp_s, 5000);
	PrintScreen("-----------------wait for module conf report response(%d) result = %d", result_w, module_conf_resp_s.result);
}

void LibAmtpcaTest::ConfigTest()
{
	CONFIG_STRU config_s;
	memset(&config_s, 0, sizeof(CONFIG_STRU));
	//		config_s.version = 100;
	sprintf(config_s.atuid_version, "a1b2c3d4e5_123");

	PrintScreen("Config report request");
	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::config), &config_s, true, 5000);
	//fprintf(stderr, "%s:%s config result = %d\n", Time().c_str(), name.c_str(), result);

	CONFIG_RESP_STRU config_resp_s;
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::config_resp), (void*)&config_resp_s, 5000);
	PrintScreen("-----------------wait for config response(%d) atuid_version = %s, result = %d, update = %d, packet_count = %d, md5 = %s"
		, result, config_resp_s.atuid_version, config_resp_s.result
		, config_resp_s.update, config_resp_s.packet_count, config_resp_s.md5);
	Sleep(3000);
}
void LibAmtpcaTest::UploadFileRequestTest()
{
	UPLOAD_FILE_STRU upload_file_s;
	memset(&upload_file_s, 0, sizeof(UPLOAD_FILE_STRU));
	sprintf(upload_file_s.file_name, "9177700220190103104005ms1.lte");
	upload_file_s.retransmit = false;
	upload_file_s.module = 2;

	PrintScreen("Upload file request");
	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file), &upload_file_s, true, 5000);
	//fprintf(stderr, "%s:%s send upload file request(%d)\n", Time().c_str(), name.c_str(), result);

	UPLOAD_FILE_RESP_STRU upload_file_resp_s;
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp), (void*)&upload_file_resp_s, 5000);
	PrintScreen("-----------------wait for upload file response(%d) result = %d", result, upload_file_resp_s.result);
}
void LibAmtpcaTest::UploadFileDataTest()
{
	int max_packet_size = 1024 * 1024;
	FILE* dtlog_f = fopen("688k.txt", "rb");
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

		PrintScreen("Upload file data request %d", packet_no);
		int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data), &upload_file_data_s, true, 5000);
		//fprintf(stderr, "%s:%s Upload file data result = %d\n", Time().c_str(), name.c_str(), result);

		UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_s;
		int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp), (void*)&upload_file_data_resp_s, 5000);
		PrintScreen("-----------------wait for upload file data response(%d) result = %d, file_id = %d, packet_no = %d"
			, result, upload_file_data_resp_s.result, upload_file_data_resp_s.file_id, upload_file_data_resp_s.packet_no);
		Sleep(10);
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

	PrintScreen("Upload eof request");
	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof), &upload_eof_s, true, 5000);
	//fprintf(stderr, "%s:%s send upload eof request(%d)\n", Time().c_str(), name.c_str(), result);

	UPLOAD_EOF_RESP_STRU upload_eof_resp_s;
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp), (void*)&upload_eof_resp_s, 5000);
	PrintScreen("-----------------wait for upload eof response(%d) result = %d"
		, result, upload_eof_resp_s.result);
	Sleep(3000);
}
void LibAmtpcaTest::LogoutTest()
{
	PrintScreen("Logout request");
	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::logout), NULL, true, 5000);
	//fprintf(stderr, "%s:%s logout result = %d\n", Time().c_str(), name.c_str(), result);
	LOGOUT_RESP_STRU logout_resp_s;
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::logout_resp), (void*)&logout_resp_s, 5000);
	PrintScreen("-----------------wait for logout response(%d) result = %d", result, logout_resp_s.result);
	Sleep(3000);
}
LIBAMTPCA_CMD_RECV_CBFUN LibamtpcaCmdRecvCbfun(AMTP_CMD_ENUM cmd, void* cmd_s, void* reserve)
{
	//PrintScreen("LibamtpcaCmdRecvCbfun recv cmd:0x%08X!", static_cast<uint32_t>(cmd));
	if (cmd == AMTP_CMD_ENUM::login_resp)
	{
		LOGIN_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGIN_RESP_STRU));
		//PrintScreen("Callback recv login response, manufacturer = %s, result = %d", resp_s.manufacturer, resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::module_conf_resp)
	{
		MODULE_CONF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(MODULE_CONF_RESP_STRU));
		//PrintScreen("Callback recv module conf report response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::config_resp)
	{
		CONFIG_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(CONFIG_RESP_STRU));
		//PrintScreen("Callback recv config response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::logout_resp)
	{
		LOGOUT_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(LOGOUT_RESP_STRU));
		//PrintScreen("Callback recv logout response, result = %d", resp_s.result);
	}
	if (cmd == AMTP_CMD_ENUM::config_data)
	{
		CONFIG_DATA_STRU* s = (CONFIG_DATA_STRU*)cmd_s;
		//PrintScreen("Callback recv config data command, atuid_version = %s, packet_count = %d, packet_no = %d, data_length = %d"
			//, s->atuid_version, s->packet_count, s->packet_no, s->data_length);

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
		//PrintScreen("config data result = %d", result);

	}
	if (cmd == AMTP_CMD_ENUM::config_notify)
	{
		CONFIG_NOTIFY_STRU* s = (CONFIG_NOTIFY_STRU*)cmd_s;
		//PrintScreen("Callback recv config notify command, atuid_version = %s, packet_count = %d"
			//, s->atuid_version, s->packet_count);

		CONFIG_NOTIFY_RESP_STRU resp_s;
		memset(&resp_s, 0, sizeof(CONFIG_NOTIFY_RESP_STRU));
		resp_s.result = 0;
		memcpy(resp_s.atuid_version, s->atuid_version, sizeof(s->atuid_version));

		int result = lib_sendcmd_cb(static_cast<uint32_t>(AMTP_CMD_ENUM::config_notify_resp), &resp_s, false, 5000);
		//PrintScreen("Config notify resp result = %d", result);
	}
	if (cmd == AMTP_CMD_ENUM::upload_file_resp)
	{
		UPLOAD_FILE_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_FILE_RESP_STRU));
		/*PrintScreen("Callback recv upload file response, result = %d, file_name = %s, file_id = %d, file_size = %d, module = %d"
			, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.file_size, resp_s.module);*/
	}
	if (cmd == AMTP_CMD_ENUM::upload_eof_resp)
	{
		UPLOAD_EOF_RESP_STRU resp_s;
		memcpy(&resp_s, cmd_s, sizeof(UPLOAD_EOF_RESP_STRU));
		/*PrintScreen("Callback recv upload eof response, result = %d, file_name = %s, file_id = %d, module = %d"
			, resp_s.result, resp_s.file_name, resp_s.file_id, resp_s.module);*/
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
		/*PrintScreen("Callback recv upload file data response, result = %d, file_id = %d, packet_no = %d"
			, resp_s.result, resp_s.file_id, resp_s.packet_no);*/
	}
	return 0;
}

void LibAmtpcaTest::MultithreadingUploadFile()
{
	int thread_count = 0;
	int packet_size = 0;
	printf("please input count of thread(1 - 20): ");
	scanf("%d", &thread_count);
	printf("please input packet size(unit 512KB): ");
	scanf("%d", &packet_size);
	if (thread_count < 1 || thread_count > 20)
	{
		fprintf(stderr, "%s: the input count is invalid.\n", Time().c_str());
		return;
	}
	thread t[20];
	for (int x = 0; x < thread_count; x++)
	{
		t[x] = thread(&LibAmtpcaTest::MultithreadingUploadFileThread, this, x, packet_size);
		Sleep(50);
	}
	for (int x = 0; x < thread_count; x++)
	{
		if (t[x].joinable())
		{
			t[x].join();
		}
	}
	fprintf(stderr, "%s: Multithreading UploadFile end.\n", Time().c_str());
}
string uplaod_file_name(int index)
{
	char time_buf[64];
	char buf[64];

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms

	//gettimeofday(&tv, NULL);
	sprintf_s(buf, sizeof(buf) - 1, "%04d-%02d-%02d-%02d-%02d-%02d"
		, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf(time_buf, "%s_%d.txt", buf, index);
	return buf;
}
char* randstr(char* str, const int len)
{
	srand((unsigned int)time(NULL));
	int i;
	for (i = 0; i < len; ++i)
	{
		switch ((rand() % 3))
		{
		case 1:
			str[i] = 'A' + rand() % 26;
			break;
		case 2:
			str[i] = 'a' + rand() % 26;
			break;
		default:
			str[i] = '0' + rand() % 10;
			break;
		}
	}
	str[++i] = '\0';
	return str;
}
void LibAmtpcaTest::MultithreadingUploadFileThread(int thread_index, int packet_size)
{
	string thread_name = "thread " + to_string(thread_index);
	string file_name = uplaod_file_name(thread_index);
	fprintf(stderr, "%s:%s -----------------------start-----------------------.\n", Time().c_str(), thread_name.c_str());

	UPLOAD_FILE_STRU upload_file_s;
	memset(&upload_file_s, 0, sizeof(UPLOAD_FILE_STRU));
	sprintf(upload_file_s.file_name, file_name.c_str());
	upload_file_s.retransmit = false;
	upload_file_s.module = 2;


	UPLOAD_FILE_RESP_STRU upload_file_resp_s;
	int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file), &upload_file_s, true, 5000);
	// fprintf(stderr, "%s:%s send upload file request(%d)\n", Time().c_str(), thread_name.c_str(), handle);
	int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_resp), (void*)&upload_file_resp_s, 5000);
	uint32_t file_id = upload_file_resp_s.file_id;
	fprintf(stderr, "%s:%s -----------------------------wait for upload file response(%d) result = %d, file_id = %u, file_name = %s\n",
		Time().c_str(), thread_name.c_str(), result, upload_file_resp_s.result, file_id, upload_file_resp_s.file_name);

	int max_packet_size = packet_size * 512 * 1024;
	int total_size = max_packet_size * 20;

	// packet_count = ((int)file_size / max_packet_size) + (((int)(file_size % max_packet_size) == 0) ? 0 : 1);
	uint32_t packet_count = 20;

	Sleep(1000);
	for (uint32_t packet_no = 1; packet_no <= packet_count; packet_no++)
	{
		int read_len = 0;
		// if (packet_no == packet_count)
		// {
		// 	read_len = (int)file_size - (packet_no - 1) * max_packet_size;
		// }
		// else
		// {
		// 	read_len = max_packet_size;
		// }
		read_len = max_packet_size;

		// fseek(dtlog_f, (packet_no - 1) * max_packet_size, SEEK_SET);
		// unique_ptr<unsigned char[]> buf(new unsigned char[read_len]());
		// fread(buf.get(), 1, read_len, dtlog_f);
		unique_ptr<unsigned char[]>buf(new unsigned char[read_len]());
		// memset(buf.get(), packet_no, read_len);
		//randstr((char*)buf.get(), read_len);

		UPLOAD_FILE_DATA_STRU upload_file_data_s;
		memset(&upload_file_data_s, 0, sizeof(UPLOAD_FILE_DATA_STRU));
		upload_file_data_s.file_id = file_id;
		upload_file_data_s.packet_no = packet_no;
		upload_file_data_s.data_len = read_len;
		upload_file_data_s.data = buf.get();

		UPLOAD_FILE_DATA_RESP_STRU upload_file_data_resp_s;
		int handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data), &upload_file_data_s, true, 5000);
		fprintf(stderr, "%s:%s Upload file data result = %d\n", Time().c_str(), thread_name.c_str(), handle);
		if (handle >= 0)
		{
			int result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_file_data_resp), (void*)&upload_file_data_resp_s, 10000);
			if (result < 0)
			{
				fprintf(stderr, "%s:%s -----------------------------wait for upload file data response(%d, %d) result = %d, file_id = %u, packet_no = %d\n",
					Time().c_str(), thread_name.c_str(), result, handle, upload_file_data_resp_s.result, upload_file_data_resp_s.file_id, upload_file_data_resp_s.packet_no);
			}
		}
		else
		{
			fprintf(stderr, "%s:%s Upload file data result = %d\n", Time().c_str(), thread_name.c_str(), handle);

		}
		 Sleep(50);
		// std::this_thread::sleep_for(std::chrono::seconds(1));
		/*std::this_thread::sleep_for(std::chrono::milliseconds(50));
		std::this_thread::yield();*/
	}
	// fclose(dtlog_f);

	Sleep(3000);

	UPLOAD_EOF_STRU upload_eof_s;
	memset(&upload_eof_s, 0, sizeof(UPLOAD_EOF_STRU));
	sprintf(upload_eof_s.file_name, file_name.c_str());
	upload_eof_s.file_id = file_id;
	upload_eof_s.packet_count = packet_count;
	upload_eof_s.total_size = total_size;
	upload_eof_s.module = 3;
	sprintf(upload_eof_s.md5, "abcdefghijklmnopqrstuvwxyz98765");


	UPLOAD_EOF_RESP_STRU upload_eof_resp_s;
	handle = lib_sendcmd(static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof), &upload_eof_s, true, 5000);
	// fprintf(stderr, "%s:%s send upload eof request(%d)\n", Time().c_str(), thread_name.c_str(), handle);
	result = lib_waitforcmd(handle, static_cast<uint32_t>(AMTP_CMD_ENUM::upload_eof_resp), (void*)&upload_eof_resp_s, 5000);
	fprintf(stderr, "%s:%s -----------------------------wait for upload eof response(%d) result = %d\n", Time().c_str(), thread_name.c_str(), result, upload_eof_resp_s.result);
	fprintf(stderr, "%s:%s -----------------------end-----------------------.\n", Time().c_str(), thread_name.c_str());
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