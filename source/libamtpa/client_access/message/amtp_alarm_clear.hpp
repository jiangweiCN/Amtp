//
//  amtp_alarm_clear.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_alarm_clear_hpp
#define amtp_alarm_clear_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_alarm_clear
{
public:
	amtp_alarm_clear();
	~amtp_alarm_clear(void);
	
	unique_ptr<JwumqMessage> data(ALARM_CLEAR_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_alarm_clear_hpp */
