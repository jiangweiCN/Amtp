//
//  proxy_config.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef proxy_config_hpp
#define proxy_config_hpp

#include <stdio.h>
#include "config.h"
#include <string>
#include <memory>

using namespace std;

using PROXY_CONFIG_S = struct proxy_config_s
{
	string frontend_type;
	string backend_type;
	bool frontend_bind;
	bool backend_bind;
	string frontend_address;
	string backend_address;
};

class ProxyConfig:public Config
{
public:
	ProxyConfig(string conf_file, string id);
	~ProxyConfig(void);
	
	bool Parse();
	string Print();
private:
	
public:
	unique_ptr<PROXY_CONFIG_S> config_p;
private:

};

#endif /* proxy_config_hpp */
