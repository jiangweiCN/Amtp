//
//  libjwumq.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumq_hpp
#define libjwumq_hpp

#include <stdio.h>
#include "../jwumq/jwumq_define.h"

extern "C"
{
	const char * jwumq_version();
	void * jwumq_dealer_load();
	int jwumq_setup(void * handle, JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK callback);
	void jwumq_release(void * handle);
	int jwumq_send(void * handle, void * msg);
	/*for C, Go*/
	int jwumq_send_c(void * handle, int command, char * from, char * to, void * data, int data_len);
	int jwumq_setup_c(void * handle, JWUMQ_SETUP_CONF_T * setup_conf, JWUMQ_RECV_CALLBACK_C callback);
}

#endif /* libjwumq_hpp */
