//
//  amtp_restart_cmd_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_restart_cmd_resp_hpp
#define amtp_restart_cmd_resp_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_restart_cmd_resp
{
public:
	amtp_restart_cmd_resp();
	~amtp_restart_cmd_resp(void);
	
	unique_ptr<JwumqMessage> data(RESTART_CMD_RESP_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_restart_cmd_resp_hpp */
