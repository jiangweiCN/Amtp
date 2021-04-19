//
//  amtp_config_notify_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/5.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_config_notify_resp_hpp
#define amtp_config_notify_resp_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_config_notify_resp
{
public:
	amtp_config_notify_resp();
	~amtp_config_notify_resp(void);
	
	unique_ptr<JwumqMessage> data(CONFIG_NOTIFY_RESP_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};


#endif /* amtp_config_notify_resp_hpp */
