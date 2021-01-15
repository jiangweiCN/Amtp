//
//  amtp_query_data.hpp
//  Amtp
//
//  Created by jiangwei on 2020/3/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef amtp_query_data_hpp
#define amtp_query_data_hpp

#include <stdio.h>
#include <memory>
#include "../amtpca_define.h"
#include "../../../jwumq/jwumq_message.hpp"
#include "../../libamtpa_define.h"


using namespace std;


class amtp_query_data
{
public:
	amtp_query_data();
	~amtp_query_data(void);
	
	bool data(JwumqMessage * msg, QUERY_DATA_STRU *s);
	void token(unsigned char *token_buf);
	
private:
	
public:
	
private:
	unsigned char _token[TOKEN_BUFFER_SIZE];
};


#endif /* amtp_query_data_hpp */
