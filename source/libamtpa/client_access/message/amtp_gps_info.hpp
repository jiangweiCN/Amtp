//
//  amtp_gps_info.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_gps_info_hpp
#define amtp_gps_info_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_gps_info
{
public:
	amtp_gps_info();
	~amtp_gps_info(void);
	
	unique_ptr<JwumqMessage> data(GPS_INFO_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_gps_info_hpp */
