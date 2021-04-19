//
//  amtp_upload_file_data.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/11.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_upload_file_data_hpp
#define amtp_upload_file_data_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_upload_file_data
{
public:
	amtp_upload_file_data();
	~amtp_upload_file_data(void);
	
	unique_ptr<JwumqMessage> data(UPLOAD_FILE_DATA_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_upload_file_data_hpp */
