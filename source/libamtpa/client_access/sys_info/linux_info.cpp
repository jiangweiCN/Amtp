//
//  linux_info.cpp
//  Amtp
//
//  Created by jiangwei on 2020/8/31.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "linux_info.hpp"
#include <stdint.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>


unsigned int veax;
unsigned int vebx;
unsigned int vedx;
unsigned int vecx;

LinuxInfo::LinuxInfo()
{
	
}

LinuxInfo::~LinuxInfo()
{
	
}

SYS_INFO_T LinuxInfo::Get()
{
	SYS_INFO_T info;
	
	info.system = GetOSInfo();
	info.cpu_id = GetCpuid();
	info.hard_disk = GetHardDiskInfo();
	return info;
}

void LinuxInfo::Cpuid(unsigned int veax1)
{
    asm("cpuid"
        :"=a"(veax),
        "=b"(vebx),
        "=c"(vecx),
        "=d"(vedx)
        :"a"(veax));
}
void LinuxInfo::Lm(int var,uint32_t *vx)
{
  int i;
  for(i=0;i<3;i++)
  {
      var=(var>>i);
      vx[i]=var;
  }
}

string LinuxInfo::GetCpuid ()
{
    uint32_t ax[3],cx[3],dx[3];
    Cpuid(1);
    Lm(veax,ax);
    Cpuid(3);
    Lm(vecx,cx);
    Lm(vedx,dx);
	char id[512] = {0};
    sprintf(id,"%u%u%u%u%u%u%u%u%u",ax[0],ax[1],ax[2],cx[0],cx[1],cx[2],dx[0],dx[1],dx[2]);
	return id;
}

string LinuxInfo::GetOSInfo()
{
	struct utsname u;
	uname(&u);
	string info = u.sysname;
	info += ", ";
	info += u.version;
	info += ", ";
	info += u.release;
	
	return info;
}
string LinuxInfo::GetHardDiskInfo()
{
	struct hd_driveid hd;
	int fd;
	string info = "";
	
	if ((fd = open("/dev/sda", O_RDONLY | O_NONBLOCK)) < 0)
	{
//		printf("ERROR opening /dev/sda\n");
		return info;
	}
	
	if (!ioctl(fd, HDIO_GET_IDENTITY, &hd))
	{
		info = (char*)hd.model;
		info += ",";
		info += (char*)hd.serial_no;
	}
	return info;

}
