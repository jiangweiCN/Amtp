//
//  libamtpsa.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/7.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libamtpsa_hpp
#define libamtpsa_hpp

#include <stdio.h>
#include "../libamtpa_define.h"

extern "C"
{
	const char * amtpsa_version();
	void * amtpsa_load();
	int amtpsa_connection(void * handle, char * mq_id, char * cmd_address, char * data_address);
	void amtpsa_release(void * handle);
	int amtpsa_sendmsg(void * handle, uint32_t cmd, char * des_id, void * data, int data_len);
	int amtpsa_readmsg(void * handle, char * src_id, void * data, int data_len);
	int amtpsa_waitformsg(void * handle, int * msg_len, int timeout);
	int amtpsa_setlimit(void * handle, int max_size);
}

#endif /* libamtpsa_hpp */
