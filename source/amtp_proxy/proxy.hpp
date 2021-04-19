//
//  proxy.hpp
//  Amtp
//
//  Created by jiangwei on 2020/5/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef proxy_hpp
#define proxy_hpp

#include <stdio.h>
#include <string>
#include <memory>
#include "../config/proxy_config.hpp"
#include "../jwumq_external/jwumq_proxy.hpp"

using namespace std;

class Proxy
{
public:
	Proxy(string conf_file, string process_id);
	~Proxy(void);
	
	bool LoadConfig();
	int Setup();
	void Release();
private:

public:
	
private:
	string process_id;
	unique_ptr<ProxyConfig> proxy_config_p;
	JwumqProxy proxy;
	unique_ptr<PROXY_CONF_T> conf_p;
};

#endif /* proxy_hpp */
