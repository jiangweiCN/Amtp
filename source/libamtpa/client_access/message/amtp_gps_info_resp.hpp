//
//  amtp_gps_info_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_gps_info_resp_hpp
#define amtp_gps_info_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_gps_info_resp
{
public:
	amtp_gps_info_resp();
	~amtp_gps_info_resp(void);
	
	GPS_INFO_RESP_STRU data(JwumqMessage * msg);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};


#endif /* amtp_gps_info_resp_hpp */
