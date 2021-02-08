//
//  libamtpa_license.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libamtpa_license.hpp"
#include <iostream>
#include <ctime>
#include <memory>


unsigned int crc_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0 };



AmtpaLicense::AmtpaLicense()
{
}

AmtpaLicense::~AmtpaLicense()
{
}

int AmtpaLicense::Load(string license_file, LIC_CONF_T &load_conf)
{
	FILE* f = fopen(license_file.c_str(), "rb");
	if (f == NULL)
	{
		return -1;
	}
	size_t size = 0;
	fseek(f,0L,SEEK_END);
	size=ftell(f);
	fseek(f,0L,SEEK_SET);
	
	unique_ptr<unsigned char[]> buf(new unsigned char[size]());
	fread( buf.get(), 1, size, f );
	fclose(f);
	
	int license_length = (int)size;
	int remainder = 8 - ((license_length % 4) + 1);
	unique_ptr<unsigned char[]> license_buf(new unsigned char[license_length]());
	for(int x = 0; x < (int)license_length; x++)
	{
		uint16_t a = buf[x] & 0x0ff;
		uint16_t b = (a >> (8-remainder)) & 0x0ff;
		a = a << remainder;
		a = a | b;
		license_buf[x] = ~(a & 0x0ff);
	}

	license_msg::License license;
	license_msg::LicenseInfo license_info;
	
	license.ParseFromArray(license_buf.get(), (int)size);

	unsigned char *info=(unsigned char*)license.info().c_str();
	int len = license.info_len();
	unsigned short crc = 0;
	unsigned char crc_H8;
	unsigned char *ptr = info;
	while (len--)
	{
		crc_H8 = (unsigned char)(crc >> 8);
		crc = crc << 8;
		crc = crc ^ crc_table[crc_H8 ^ *ptr];
		ptr++;
	}
	if(crc != license.check())
	{
		return -1;
	}
	
	license_info.ParseFromArray(license.info().c_str(), (int)license.info().size());
	
	load_conf.lic_type = license.type();
	load_conf.check = license.check();
	load_conf.cmd_ip = license_info.cmd_ip();
	load_conf.cmd_port = license_info.cmd_port();
	load_conf.data_ip = license_info.data_ip();
	load_conf.data_port = license_info.data_port();
	load_conf.box_id = license_info.box_id();
	load_conf.pwd = license_info.pwd();
	load_conf.manufactor = license_info.manufactor();
	load_conf.begin_time = license_info.begin_time();
	load_conf.end_time = license_info.end_time();
	load_conf.release_id = license_info.release_id();
	load_conf.jwumq_id = license_info.jwumq_id();
	load_conf.save_path = "";
	return 0;
}

int AmtpaLicense::Make(LIC_CONF_T lic_conf)
{
	string license_file = lic_conf.save_path + "/" + lic_conf.box_id + ".license";
	return Make(lic_conf, license_file);
}

int AmtpaLicense::Make(LIC_CONF_T lic_conf, string file)
{
	string license_file = file;
	
	license_msg::License license;
	license_msg::LicenseInfo license_info;
	
	license.set_type(license_msg::LicenseType::dev);
	if(lic_conf.lic_type == 0)
	{
		license.set_type(license_msg::LicenseType::dev);
	}
	else if(lic_conf.lic_type == 1)
	{
		license.set_type(license_msg::LicenseType::atu);
	}

	license_info.set_cmd_ip(lic_conf.cmd_ip);
	license_info.set_cmd_port(lic_conf.cmd_port);
	license_info.set_data_ip(lic_conf.data_ip);
	license_info.set_data_port(lic_conf.data_port);
	license_info.set_box_id(lic_conf.box_id);
	license_info.set_pwd(lic_conf.pwd);
	license_info.set_manufactor(lic_conf.manufactor);
	license_info.set_begin_time(lic_conf.begin_time);
	license_info.set_end_time(lic_conf.end_time);
	license_info.set_release_id(lic_conf.release_id);
	license_info.set_jwumq_id(lic_conf.jwumq_id);
	
	size_t license_info_len = license_info.ByteSizeLong();
	unique_ptr<unsigned char[]> buf(new unsigned char[license_info_len]());
	license_info.SerializeToArray(buf.get(), (int)license_info_len);
	
	int len = (int)license_info_len;
	unsigned short crc = 0;
	unsigned char crc_H8;
	unsigned char *ptr = buf.get();
	while (len--)
	{
		crc_H8 = (unsigned char)(crc >> 8);
		crc = crc << 8;
		crc = crc ^ crc_table[crc_H8 ^ *ptr];
		ptr++;
	}
	
	license.set_info_len((int)license_info_len);
	license.set_check(crc);
	
	license.set_info(buf.get(), license_info_len);
	
	if(Save(license_file, license) == 0)
	{
		fprintf(stderr, "OK, license file: %s!\n", license_file.c_str());
	}
	else
	{
		fprintf(stderr, "Error, license file:'%s'!\n", license_file.c_str());
	}
	
	return 0;
}
int AmtpaLicense::Save(string license_file, license_msg::License license)
{
	size_t license_length = license.ByteSizeLong();
	unique_ptr<unsigned char[]> license_data(new unsigned char[license_length]());
	license.SerializeToArray(license_data.get(), (int)license_length);
	
	int remainder = (license_length % 4) + 1;
	unique_ptr<unsigned char[]> license_buf(new unsigned char[license_length]());
	for(int x = 0; x < (int)license_length; x++)
	{
		uint16_t a = license_data[x] & 0x0ff;
		uint16_t b = (a >> (8-remainder)) & 0x0ff;
		a = a << remainder;
		a = a | b;
		license_buf[x] = ~(a & 0x0ff);
	}
#if defined(_WIN32)
	errno_t err;
	FILE* fp;
	if ((err = fopen_s(&fp, license_file.c_str(), "wb")) == 0)
	{
		fprintf(stderr, "The file 'crt_fopen_s.c' was opened!\n");
		return -1;
	}
#else
	FILE* fp = fopen(license_file.c_str(), "wb");
#endif
	
	if (fp == NULL)
	{
		return -1;
	}
	fwrite(license_buf.get(), license_length, 1, fp);
	fclose(fp);

	return 0;
}

