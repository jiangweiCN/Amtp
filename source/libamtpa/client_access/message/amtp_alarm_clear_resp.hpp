//
//  amtp_alarm_clear_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_alarm_clear_resp_hpp
#define amtp_alarm_clear_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_alarm_clear_resp
{
public:
	amtp_alarm_clear_resp();
	~amtp_alarm_clear_resp(void);
	
	ALARM_CLEAR_RESP_STRU data(JwumqMessage * msg);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};

#endif /* amtp_alarm_clear_resp_hpp */