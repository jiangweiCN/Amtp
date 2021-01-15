//
//  JLog.cpp
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/7.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#include "jLog.h"


#if defined(_WIN32)
#include <io.h>
#include <direct.h>
#include <fstream>
#include "../include/glog_win/logging.h"
//#include "../include/glog_win/raw_logging.h"
//using namespace google;

#ifdef _DEBUG
#pragma comment(lib, "../../source/include/glog_win/glogd.lib")
#else
#pragma comment(lib, "../../source/include/glog_win/glog.lib")
#endif // DEBUG

#else
#include "../include/glog/logging.h"
#include "../include/glog/raw_logging.h"
//#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <unistd.h>        //access()
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>

#endif

using namespace std;
#define BUF_LEN 1024

bool JLog::active = false;//静态成员在此初始化
void SignalHandle(const char* data, int size);

JLog::JLog(void)
{
    
}

JLog::~JLog(void)
{
    
}

void JLog::Init(const char * process_name, int logtostderr, const char * log_path)
{
    if(active)
        return;
    
    CreateMultiLevelDir(log_path);
    
    google::InitGoogleLogging(process_name);    // in
    FLAGS_logtostderr = logtostderr;  //1:stderr, 0:file
    FLAGS_max_log_size = 10;            //file size
    FLAGS_log_dir = log_path;
	FLAGS_logbufsecs =0;
	FLAGS_colorlogtostderr=true;
	google::InstallFailureSignalHandler();
	//默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，可以通过下面的方法自定义输出方式：
	google::InstallFailureWriter(&SignalHandle);
    active = true;
}

int JLog::CreateMultiLevelDir(const char* sPathName)
{
    char DirName[256]; 
    int i, len;
    
#if defined(_WIN32)
	strcpy_s(DirName, sPathName);
#else
	strcpy(DirName, sPathName);
#endif
    
    len = (int)strlen(DirName);
    if('/' != DirName[len-1]) 
	{
#if defined(_WIN32)
		strcat_s(DirName, "/");
#else
		strcat(DirName, "/");
#endif
        len++;
    }
    
    for(i=1; i<len; i++)
    {
        if('/' == DirName[i])
        {
            DirName[i] = '\0';
#if defined(_WIN32)
			if (_access(DirName, 0) != 0)
			{
				if (_mkdir(DirName) == -1)
				{
					perror("mkdir() failed!");
					return -1;
				}
		}
#else
			if (access(DirName, F_OK) != 0)
			{
				if (mkdir(DirName, 0777) == -1)
				{
					perror("mkdir() failed!");
					return -1;
				}
			}
#endif
            
            DirName[i] = '/';
        }
    }
    
    return 0;
}

void JLog::Close()
{
    google::ShutdownGoogleLogging();
    active = false;
}

void SignalHandle(const char* data, int size)
{
	std::ofstream fs("glog_dump.log",std::ios::app);
	std::string str = std::string(data,size);
	fs<<str;
	fs.close();
	LOG(ERROR)<<str;
}


