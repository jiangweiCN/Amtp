//
//  amtp_login.hpp
//  Amtp
//
//  Created by jiangwei on 2020/2/29.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_login_hpp
#define amtp_login_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_login
{
public:
	amtp_login();
	~amtp_login(void);
	
	unique_ptr<JwumqMessage> data(LOGIN_STRU * login_s, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_login_hpp */
