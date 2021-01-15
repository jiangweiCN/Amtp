//
//  router.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/3.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef router_hpp
#define router_hpp

#include <stdio.h>
#include <string>
#include <memory>
#include "../config/router_config.hpp"
#include "../jwumq/jwumq_router.hpp"


using namespace std;

class Router
{
public:
	Router(string conf_file, string process_id);
	~Router(void);
	
	bool LoadConfig();
	int Setup();
	void Release();
private:

public:
	
private:
	string process_id;
	unique_ptr<RouterConfig> router_config_p;
	JwumqRouter router;
	unique_ptr<ROUTER_CONF_T> conf_p;
};

#endif /* router_hpp */
