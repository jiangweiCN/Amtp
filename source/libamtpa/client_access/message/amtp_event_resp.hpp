//
//  amtp_event_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_event_resp_hpp
#define amtp_event_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_event_resp
{
public:
	amtp_event_resp();
	~amtp_event_resp(void);
	
	REPORT_EVENT_RESP_STRU data(JwumqMessage * msg);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};

#endif /* amtp_event_resp_hpp */
