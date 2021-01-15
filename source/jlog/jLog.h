//
//  JLog.h
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/7.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#ifndef jLog_h
#define jLog_h

#include <stdio.h>

#if defined(_WIN32)
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "../include/glog_win/logging.h"
#else
#include "../include/glog/logging.h"
#include "../include/glog/raw_logging.h"
#endif



#define JLOG LOG

enum _JLOG_
{
    JLOG_INFO = 0, JLOG_WARNING, JLOG_ERROR, JLOG_FATAL
};

class JLog
{
public:
    JLog(void);
    ~JLog(void);
public:
    static void Init(const char * process_name, int logtostderr = 1, const char * log_path = "log");
    static void Close();
    
//    static void Log(_JLOG_ type, const char *fmt, ...);
private:
    static int CreateMultiLevelDir(const char* sPathName);
    
private:
    static bool active;
};

#endif /* JLog_h */
