//
//  amtp_alarm.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/2.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_alarm_hpp
#define amtp_alarm_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_alarm
{
public:
	amtp_alarm();
	~amtp_alarm(void);
	
	unique_ptr<JwumqMessage> data(ALARM_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_alarm_hpp */
