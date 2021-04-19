//
//  amtp_module_conf.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/25.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_module_conf_hpp
#define amtp_module_conf_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_module_conf
{
public:
	amtp_module_conf();
	~amtp_module_conf(void);
	
	unique_ptr<JwumqMessage> data(MODULE_CONF_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	

};

#endif /* amtp_module_conf_hpp */
