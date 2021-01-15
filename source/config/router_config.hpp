//
//  router_config.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef router_config_hpp
#define router_config_hpp

#include <stdio.h>
#include "config.h"
#include <string>
#include <memory>

using namespace std;

using ROUTER_CONFIG_S = struct router_config_s
{
	string frontend_type;
	string backend_type;
	bool frontend_bind;
	bool backend_bind;
	string frontend_address;
	string backend_address;
};

class RouterConfig:public Config
{
public:
	RouterConfig(string conf_file, string id);
	~RouterConfig(void);
	
	bool Parse();
	string Print();
private:
	
public:
	unique_ptr<ROUTER_CONFIG_S> config_p;
private:

};
#endif /* router_config_hpp */
