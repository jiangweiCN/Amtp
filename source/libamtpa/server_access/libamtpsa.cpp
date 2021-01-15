//
//  libamtpsa.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/7.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libamtpsa.hpp"
#include "amtpsa.hpp"
#include <memory>


using namespace std;

constexpr auto libamtpsa_version = "libamtpsa version 1.0.3.20200402";


const char * amtpsa_version()
{
	return libamtpsa_version;
}

void * amtpsa_load()
{
	Amtpsa * handle = new Amtpsa();
//	fprintf(stderr, "Amtpsa * handle = %016X!\n", handle);
	return (void *)handle;
}

int amtpsa_connection(void * handle, char * mq_id, char * cmd_address, char * data_address)
{
	if(handle == nullptr || mq_id == nullptr || cmd_address == nullptr || data_address == nullptr)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	
	return amtpsa_p->Connection(mq_id, cmd_address, data_address);
}

void amtpsa_release(void * handle)
{
	if(handle == nullptr)
	{
		return;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	amtpsa_p->Release();
}

int amtpsa_sendmsg(void * handle, uint32_t cmd, char * des_id, void * data, int data_len)
{
	if(handle == nullptr || des_id == nullptr || data == nullptr || data_len <= 0)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	return amtpsa_p->SendCmd(cmd, des_id, data, data_len);
}

int amtpsa_readmsg(void * handle, char * src_id, void * data, int data_len)
{
	if(handle == nullptr || src_id == nullptr || data == nullptr || data_len <= 0)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	return amtpsa_p->ReadCmd(src_id, data, data_len);
}

int amtpsa_waitformsg(void * handle, int * msg_len, int timeout)
{
	if(handle == nullptr || msg_len == nullptr)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	return amtpsa_p->WaitForCmd(msg_len, timeout);
}
int amtpsa_setlimit(void * handle, int max_size)
{
	if(handle == nullptr)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	Amtpsa * amtpsa_p = (Amtpsa *) handle;
	return amtpsa_p->SetLimit(max_size);
}



