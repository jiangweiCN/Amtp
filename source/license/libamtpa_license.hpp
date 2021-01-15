//
//  libamtpa_license.hpp
//  Amtp
//
//  Created by jiangwei on 2020/8/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libamtpa_license_hpp
#define libamtpa_license_hpp

#include <stdio.h>
#include <string>
#include "license_msg.pb.h"


using namespace std;

using LIC_CONF_T = struct lic_conf_s
{
	uint32_t	lic_type;
	uint32_t	check;
	string		cmd_ip;
	uint32_t	cmd_port;
	string		data_ip;
	uint32_t	data_port;
	string		box_id;
	string		pwd;
	string		manufactor;
	uint64_t	begin_time;//seconds from 1970-1-1
	uint64_t	end_time;//seconds from 1970-1-1
	uint32_t	release_id;
	string		jwumq_id;
	string		save_path;
};

class AmtpaLicense
{
public:
	AmtpaLicense();
	~AmtpaLicense(void);
	
	int Load(string license_file, LIC_CONF_T &load_conf);
	int Make(LIC_CONF_T lic_conf);

private:
	int Save(string license_file, license_msg::License license);

	
public:
//	license_msg::License license;
//	license_msg::LicenseInfo license_info;
private:
	
};

#endif /* libamtpa_license_hpp */
