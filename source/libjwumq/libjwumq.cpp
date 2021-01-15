//
//  libjwumq.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libjwumq.hpp"
#include "../jwumq/jwumq_dealer.hpp"
#include "../jwumq/jwumq_define.h"
#include <memory>


using namespace std;


constexpr auto libamtpa_version = "version 1.1.0.20200609";


const char * jwumq_version()
{
	return libamtpa_version;
}

void * jwumq_dealer_load()
{
	JwumqDealer * dealer = new JwumqDealer();
	return dealer;
}

int jwumq_setup(void * handle, JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback)
{
	if(setup_conf == nullptr || callback == nullptr || handle == nullptr)
	{
		return LIB_JWUMQ_PARA_ABNORMAL;
	}
	
	JwumqBase * jwumq_p = (JwumqBase *)handle;
	return jwumq_p->Setup(setup_conf, callback);
}

int jwumq_setup_c(void * handle, JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback)
{
	if(setup_conf == nullptr || callback == nullptr || handle == nullptr)
	{
		return LIB_JWUMQ_PARA_ABNORMAL;
	}
	
	JwumqBase * jwumq_p = (JwumqBase *)handle;
	return jwumq_p->Setup(setup_conf, callback);
}
void jwumq_release(void * handle)
{
	if(handle != nullptr)
	{
		JwumqBase * jwumq_p = (JwumqBase *)handle;
		jwumq_p->Release();
	}
}
int jwumq_send(void * handle, void * msg)
{
	if(msg == nullptr || handle == nullptr)
	{
		return LIB_JWUMQ_PARA_ABNORMAL;
	}
	JwumqBase * jwumq_p = (JwumqBase *)handle;
	return jwumq_p->Send(msg);
}
int jwumq_send_c(void * handle, int command, char * from, char * to, void * data, int data_len)
{
	if(handle == nullptr || from == nullptr || to == nullptr || data == nullptr)
	{
		return LIB_JWUMQ_PARA_ABNORMAL;
	}
	JwumqBase * jwumq_p = (JwumqBase *)handle;
	return jwumq_p->Send(command, from, to, data, data_len);
}



