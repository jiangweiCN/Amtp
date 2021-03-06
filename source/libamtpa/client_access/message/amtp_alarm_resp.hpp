//
//  amtp_alarm_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_alarm_resp_hpp
#define amtp_alarm_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_alarm_resp
{
public:
	amtp_alarm_resp();
	~amtp_alarm_resp(void);
	
	ALARM_RESP_STRU data(JwumqMessage * msg);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};


#endif /* amtp_alarm_resp_hpp */
