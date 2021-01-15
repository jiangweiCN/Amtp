//
//  main.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//


#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string>
#include "../jlog/jLog.h"
#include "router.hpp"


using namespace std;


int InitDaemon();
int BlockSignal();
void SigHandler(int iSignal);

int main(int argc, const char * argv[])
{
	// insert code here...
	std::cout << "Hello, amtp_router!\n";
	
	if(argc < 4)
	{
		exit(1);
	}
	
	InitDaemon();
	BlockSignal();
	
	string proc_name = argv[0];
	string proc_id = argv[1];
	string config_file = argv[2];
	string log_path = argv[3];
	
	string log_name = proc_name + "_" + proc_id;
	
	log_path.append("/");
	log_path.append(log_name);
	
	JLog::Init(log_name.c_str(), 0, log_path.c_str());
	
	JLOG(INFO) << "amtp_router begin!";

	unique_ptr<Router> router(new Router(config_file, proc_id));
	if(!router->LoadConfig())
	{
		JLOG(INFO) << "Config error!";
		JLog::Close();
		exit(1);
	}
	router->Setup();
	bool loop = true;
	while (loop)
	{
		sleep(1);
	}
	router->Release();
	
	JLOG(INFO) << "amtp_router end!";
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





