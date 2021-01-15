//
//  libjwumqa_connection.hpp
//  Amtp
//
//  Created by jiangwei on 2020/6/19.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_connection_hpp
#define libjwumqa_connection_hpp

#include <stdio.h>
#include "libjwumqa_cpp.h"
#include "factory_connection.hpp"


class JwumqConnectionImpl:public JwumqConnection
{
public:
	JwumqConnectionImpl(string url);
	~JwumqConnectionImpl(void);
	
	JwumqSession* CreateSession();
	int Connect();
	void Close();
	void DestroySession(JwumqSession*);
private:

private:
	string url;
	shared_ptr<FactoryConnection> factory_conn;
};
#endif /* libjwumqa_connection_hpp */
