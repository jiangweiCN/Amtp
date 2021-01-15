//
//  amtpa_lic_generator.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <dirent.h>
#include <sys/time.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include "license_msg.pb.h"
#include "libamtpa_license.hpp"



using namespace std;

void SplitString(const string& s, vector<string>& v, const string& c);


int main(int argc, char * argv[])
{
	fprintf(stderr, "*********************Amtpa license generator begin*********************\n");
	AmtpaLicense al;
	LIC_CONF_T conf;
	char input_buf[1024] = {0};
	
	printf("please input license type(dev = 0, atu = 1):");
	scanf("%d",&conf.lic_type);
	
	memset(input_buf, 0, 1024);
	printf("please input command ip:");
	scanf("%s",input_buf);
	conf.cmd_ip = input_buf;
	
	printf("please input command port:");
	scanf("%d",&conf.cmd_port);
	
	memset(input_buf, 0, 1024);
	printf("please input data ip:");
	scanf("%s",input_buf);
	conf.data_ip = input_buf;

	printf("please input data port:");
	scanf("%d",&conf.data_port);

	memset(input_buf, 0, 1024);
	printf("please input box id:");
	scanf("%s",input_buf);
	string box_id = input_buf;
	vector<string> box_v;
	SplitString(box_id, box_v,",");

	memset(input_buf, 0, 256);
	printf("please input manufactor:");
	scanf("%s",input_buf);
	conf.manufactor = input_buf;

	{
		memset(input_buf, 0, 256);
		printf("please input the end date(YYYY-MM-DD):");
		scanf("%s",input_buf);
		string end_data_str = input_buf;
		vector<string> v;
		SplitString(end_data_str, v,"-");

		struct timeval tv;
		gettimeofday(&tv, NULL);
		conf.begin_time = tv.tv_sec;
		conf.release_id = (uint32_t)tv.tv_sec;

		if(v.size() < 3)
		{
			fprintf(stderr, "The format of end date error!\n");
			return -1;
		}
		struct tm p = {0};
		p.tm_year = stoi(v[0]) - 1900;
		p.tm_mon = stoi(v[1]) - 1;
		p.tm_mday = stoi(v[2]);

		conf.end_time = mktime(&p);
		if(conf.end_time <= conf.begin_time)
		{
			fprintf(stderr, "The end date error!\n");
			return -1;
		}
	}


	memset(input_buf, 0, 256);
	printf("please input the path for saving license:");
	scanf("%s",input_buf);
	string save_path = input_buf;
	
	DIR *dir;
	if ((dir=opendir(save_path.c_str())) == NULL)
	{
		fprintf(stderr, "The path for saving is not exist!\n");
		return -1;
	}
	closedir(dir);
	time_t now = time(0);
	tm *ltm = localtime(&now);
	conf.save_path = save_path + "/" + conf.manufactor + "-"
	+ to_string(1900 + ltm->tm_year) + "-"
	+ to_string(1 + ltm->tm_mon) + "-"
	+ to_string(ltm->tm_mday) + "-"
	+ to_string(ltm->tm_hour) + "-"
	+ to_string(ltm->tm_min) + "-"
	+ to_string(ltm->tm_sec);
	if(mkdir(conf.save_path.c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO) != 0)
	{
		printf("create path:%s error!\n",conf.save_path.c_str());
		return -1;
	}
	printf("create path:%s!\n",conf.save_path.c_str());
	
	for(int x = 0; x < (int)box_v.size(); x++)
	{
		conf.box_id = box_v[x];
		conf.jwumq_id = box_v[x];
		conf.pwd = box_v[x];

		int result = al.Make(conf);
		if(result != 0)
		{
			fprintf(stderr, "License generate error!\n");
			return -1;
		}
	}
	
	
	AmtpaLicense al_read;
	LIC_CONF_T load_conf;
	
	for(int x = 0; x < (int)box_v.size(); x++)
	{
		string license_file = conf.save_path + "/" + box_v[x] + ".license";
		
		if(al_read.Load(license_file, load_conf) != 0)
		{
			fprintf(stderr, "License load '%s' faild!\n", license_file.c_str());
			continue;
		}
		fprintf(stderr, "License load '%s' !\n", license_file.c_str());
		fprintf(stderr, "type = %d, check = %d\n", load_conf.lic_type, load_conf.check);
		fprintf(stderr, "cmd_ip = %s, cmd_port = %d, data_ip = %s, data_port = %d, box_id = %s, pwd = %s, manufactor = %s, "\
				"begin_time = %ld, end_time = %ld, release_id = %d, jwumq_id = %s\n"
				, load_conf.cmd_ip.c_str(), load_conf.cmd_port
				, load_conf.data_ip.c_str(), load_conf.data_port
				, load_conf.box_id.c_str(), load_conf.pwd.c_str()
				, load_conf.manufactor.c_str(), load_conf.begin_time
				, load_conf.end_time, load_conf.release_id
				, load_conf.jwumq_id.c_str());
	}
	
	 
	return 1;
}

void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

