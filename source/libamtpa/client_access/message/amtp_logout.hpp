//
//  amtp_logout.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef ampt_logout_hpp
#define ampt_logout_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_logout
{
public:
	amtp_logout();
	~amtp_logout(void);
	
	unique_ptr<JwumqMessage> data(unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_logout_hpp */
