//
//  amtp_config_notify.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_config_notify_hpp
#define amtp_config_notify_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_config_notify
{
public:
	amtp_config_notify();
	~amtp_config_notify(void);
	
	bool data(JwumqMessage * msg, CONFIG_NOTIFY_STRU *s);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};

#endif /* amtp_config_notify_hpp */
