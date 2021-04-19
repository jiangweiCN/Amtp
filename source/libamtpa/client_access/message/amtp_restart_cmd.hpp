//
//  amtp_restart_cmd.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_restart_cmd_hpp
#define amtp_restart_cmd_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_restart_cmd
{
public:
	amtp_restart_cmd();
	~amtp_restart_cmd(void);
	
	bool data(JwumqMessage * msg, RESTART_CMD_STRU *s);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};


#endif /* amtp_restart_cmd_hpp */
