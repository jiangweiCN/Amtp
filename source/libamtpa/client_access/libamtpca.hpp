//
//  libamtpca.hpp
//  Amtp
//
//  Created by jiangwei on 2020/1/7.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libamtpca_hpp
#define libamtpca_hpp

#include <stdio.h>
#include "../libamtpa_define.h"


#if defined(_WIN32)

#define AMTPCA_DLL extern "C" _declspec(dllexport) 

AMTPCA_DLL const char* __stdcall amtpca_version();
AMTPCA_DLL int __stdcall amtpca_init(const char* license_path, const char* log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back);
AMTPCA_DLL void __stdcall amtpca_release();
AMTPCA_DLL int __stdcall amtpca_sendcmd(uint32_t cmd, void* cmd_s, bool sync, uint32_t timeout);
AMTPCA_DLL int __stdcall amtpca_waitforcmd(uint32_t cmd, void* s, uint32_t timeout);

#else
extern "C"
{
	const char* amtpca_version();
	int amtpca_init(const char* license_path, const char* log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back);
	void amtpca_release();
	int amtpca_sendcmd(uint32_t cmd, void* cmd_s, bool sync, uint32_t timeout);
	int amtpca_waitforcmd(uint32_t cmd, void* s, uint32_t timeout);
}
#endif /* defined(_WIN32) */

#endif /* libamtpca_hpp */
