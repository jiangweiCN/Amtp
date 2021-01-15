//
//  main.cpp
//  Amtp
//
//  Created by jiangwei on 2020/5/14.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include "../jlog/jLog.h"
#include "monitor.hpp"

using namespace std;

int InitDaemon();
int BlockSignal();
int CheckReentry(const char *lpszFileName);
int LockSet(int fd,int type);
void SigHandler(int iSignal);


int main(int argc, char * argv[])
{
    // insert code here...
	Monitor monitor;
	std::cout << "Hello, jwumq_monitor!\n" << "\n";
	
	InitDaemon();
	BlockSignal();
	
	int iPidFd;
    string pid_name = "/tmp/jwumq_monitor.pid";
    if((iPidFd = CheckReentry(pid_name.c_str())) < 0)
    {
		std::cout << "jwumq_monitor is running!\n";
        exit(1);
    }
	
	string log_file = "/tmp/log/jwumq_monitor";
	JLog::Init(argv[0], 0, log_file.c_str());
	JLOG(INFO) << "jwumq_monitor begin!";
	
	monitor.Start();
	bool loop = true;
    while (loop)
    {
		sleep(1);
    }
	monitor.End();
    JLOG(INFO) << "jwumq_monitor end!";
    JLog::Close();
	
    return 0;
}

int InitDaemon()
{
    int iRet;
    
    iRet = fork();
    if (iRet<0)
    {
        fprintf(stderr, "Failed in fork.\n");
        return 0;
    }
    else if (iRet>0)
    {
        exit(0);
    }
    // child continues here
    // now close all extra fds
    for(iRet=getdtablesize()-1; iRet>=0; --iRet) close(iRet);
    // tty
    iRet = open("/dev/tty", O_RDWR);
    ioctl(iRet, TIOCNOTTY, 0);
    close(iRet);
    // stdin/stdout/stderr
    iRet=open("/dev/null", O_RDWR);
    dup(iRet);
    dup(iRet);
    return 0;
}

int BlockSignal()
{
    sigset_t newSigSet;
    struct sigaction sSigAction;
    int iState = 0;
    
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGHUP);
    sigaddset(&newSigSet, SIGINT);
    //sigaddset(&newSigSet, SIGTERM);
    sigaddset(&newSigSet, SIGTSTP);
    sigaddset(&newSigSet, SIGTTIN);
    sigaddset(&newSigSet, SIGTTOU);
    sigaddset(&newSigSet, SIGURG);
    sigaddset(&newSigSet, SIGPIPE);
    iState = sigprocmask(SIG_BLOCK, &newSigSet, NULL);
    
    sSigAction.sa_handler = SigHandler;
    sigemptyset(&sSigAction.sa_mask);
    sSigAction.sa_flags = 0;
    sigaction(SIGTERM, &sSigAction, NULL);
    sigaction(SIGUSR1, &sSigAction, NULL);
    sigaction(SIGUSR2, &sSigAction, 0);
    return iState;
}

void SigHandler(int iSignal)
{
    switch (iSignal)
    {
        case SIGTERM:
			JLOG(INFO) << "TERM signal received!";
            break;
        case SIGUSR2:
			JLOG(INFO) << "SIGUSR2 signal received";
            break;
        case SIGUSR1:
            JLOG(INFO) << "SIGUSR1 signal received";
            break;
        default:
            break;
    }
}

int CheckReentry(const char *lpszFileName)
{
    int iFd;
    char lpszContent[40];
//    struct flock lck;
    sprintf(lpszContent,"%d",getpid());
    iFd = open(lpszFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(iFd<0)
    {
        return -1;
    }
    if(LockSet(iFd,F_WRLCK) ==-1)
        return -1;
    if(write(iFd,lpszContent,strlen(lpszContent))<0)
    {
        return -1;
    }
    return iFd;
}

int LockSet(int fd,int type)
{
    struct flock lock;
    
    lock.l_type=type;
    lock.l_start=0;
    lock.l_whence=SEEK_SET;
    lock.l_len = 0;
    lock.l_pid=-1;
    
    fcntl(fd,F_GETLK,&lock);
    if(lock.l_type!=F_UNLCK)
    {
        if(lock.l_type == F_RDLCK)
        {
            return -1;
        }
        else if(lock.l_type == F_WRLCK)
        {
            return -1;
        }
    }
    lock.l_type = type;
    if(fcntl(fd,F_SETLKW,&lock)<0)
    {
        exit(1);
    }
    
    switch(lock.l_type)
    {
        case F_RDLCK:
            break;
        case F_WRLCK:
            break;
        case F_UNLCK:
            break;
        default:
            break;
    }
    return 0;
}

