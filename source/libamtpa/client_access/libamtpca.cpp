//
//  libamtpca.cpp
//  Amtp
//
//  Created by jiangwei on 2020/1/7.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "libamtpca.hpp"
#include "amtpca.hpp"
#include <memory>

constexpr auto libamtpca_version = "libamtpca version 1.0.3.20210104";
unique_ptr<Amtpca> amtpca_p = nullptr;

#if defined(_WIN32)
#if defined(_WIN64)
const char* __stdcall amtpca_version()
#else // _WIN32
const char* _cdecl amtpca_version()
#endif
#else
const char* amtpca_version()
#endif
{
	return libamtpca_version;
}

#if defined(_WIN32)
#if defined(_WIN64)
int __stdcall amtpca_init(const char* license_path, const char* log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back)
#else // _WIN32
int _cdecl amtpca_init(const char* license_path, const char* log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back)
#endif
#else
int amtpca_init(const char * license_path, const char * log_path, LIBAMTPCA_CMD_RECV_CBFUN call_back)
#endif
{
	if(amtpca_p != nullptr)
	{
		return LIB_AMTPA_IS_WORKING;
	}
	
	if(license_path == nullptr)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	amtpca_p = make_unique<Amtpca>();
	
	return amtpca_p->Init(license_path, log_path, call_back);
}

#if defined(_WIN32)
#if defined(_WIN64)
void __stdcall amtpca_release()
#else // _WIN32
void _cdecl amtpca_release()
#endif
#else
void amtpca_release()
#endif
{
	if(amtpca_p != nullptr)
	{
		amtpca_p->Release();
		amtpca_p = nullptr;
	}
}

#if defined(_WIN32)
#if defined(_WIN64)
int __stdcall amtpca_sendcmd(uint32_t cmd, void* cmd_s, bool sync, uint32_t timeout)
#else // _WIN32
int _cdecl amtpca_sendcmd(uint32_t cmd, void* cmd_s, bool sync, uint32_t timeout)
#endif
#else
int amtpca_sendcmd(uint32_t cmd, void * cmd_s, bool sync, uint32_t timeout)
#endif
{
	if(amtpca_p == nullptr)
	{
		return LIB_AMTPA_NO_WORKING;
	}
	return amtpca_p->SendCmd(cmd, cmd_s, sync, timeout);
}

#if defined(_WIN32)
#if defined(_WIN64)
int __stdcall amtpca_waitforcmd(uint32_t cmd, void* s, uint32_t timeout)
#else // _WIN32
int _cdecl amtpca_waitforcmd(uint32_t cmd, void* s, uint32_t timeout)
#endif
#else
int amtpca_waitforcmd(uint32_t cmd, void * s, uint32_t timeout)
#endif
{
	if(amtpca_p == nullptr)
	{
		return LIB_AMTPA_NO_WORKING;
	}
	if(s == NULL)
	{
		return LIB_AMTPA_PARA_ERROR;
	}
	return amtpca_p->WaitForCmd(cmd, s, timeout);
}


