//
//  amtp_upload_eof.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_upload_eof_hpp
#define amtp_upload_eof_hpp

#include <stdio.h>
#include <memory>
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"
#include "../../../license/libamtpa_license.hpp"


using namespace std;


class amtp_upload_eof
{
public:
	amtp_upload_eof();
	~amtp_upload_eof(void);
	
	unique_ptr<JwumqMessage> data(UPLOAD_EOF_STRU * s, unsigned char *token, LIC_CONF_T load_conf, uint32_t sn, string src_id, string des_id);
	
private:
	
public:
	
private:
	
};

#endif /* amtp_upload_eof_hpp */
