//
//  amtp_module_conf_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/25.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_module_conf_resp_hpp
#define amtp_module_conf_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_module_conf_resp
{
public:
	amtp_module_conf_resp();
	~amtp_module_conf_resp(void);
	
	void data(JwumqMessage * msg, MODULE_CONF_RESP_STRU &s);
	void token(unsigned char *token_buf);

	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};

#endif /* amtp_module_conf_resp_hpp */
