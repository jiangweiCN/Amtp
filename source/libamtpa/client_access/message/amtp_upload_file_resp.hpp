//
//  amtp_upload_file_resp.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_upload_file_resp_hpp
#define amtp_upload_file_resp_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq_external/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_upload_file_resp
{
public:
	amtp_upload_file_resp();
	~amtp_upload_file_resp(void);
	
	UPLOAD_FILE_RESP_STRU data(JwumqMessage * msg);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};


#endif /* amtp_upload_file_resp_hpp */
