//
//  config.cpp
//  NSSIMP_MQ
//
//  Created by jiangwei on 2018/12/26.
//  Copyright © 2018年 jiangwei. All rights reserved.
//

#include "config.h"


Config::Config()
{
	
}

Config::~Config()
{
	
}

bool Config::Load(char ** data)
{
	FILE *f = NULL;
	long len;
	char * buf;
	
	f=fopen(config_file.c_str(),"rb");
	if(f == NULL)
	{
		return false;
	}
	fseek(f,0,SEEK_END);
	len=ftell(f);
	fseek(f,0,SEEK_SET);
	buf=(char*)malloc(len+1);
	size_t read = fread(buf,1,len,f);
	fclose(f);
	
	if(read <= 0)
	{
		free(buf);
		return false;
	}
	buf[len] = 0;
	
	*data = buf;
	return true;
}
bool Config::Save(string content)
{
	FILE *f = NULL;
	
	f=fopen(config_file.c_str(),"wb");
	if(f == NULL)
	{
		return false;
	}
	fputs (content.c_str(),f);
	fclose (f);
	return true;
}
