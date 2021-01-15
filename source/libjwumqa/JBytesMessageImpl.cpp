//
//  JBytesMessageImpl.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "JBytesMessageImpl.hpp"
#include "../jlog/jLog.h"



JBytesMessageImpl::JBytesMessageImpl(unsigned char * data, int length)
{
	JLOG(INFO) << "JBytesMessageImpl::JBytesMessageImpl, " << length;
	SetBytes(data, length);
}

JBytesMessageImpl::JBytesMessageImpl()
{
	length = 0;
	this->data = nullptr;
	JLOG(INFO) << "JBytesMessageImpl::JBytesMessageImpl, " << length;
}

JBytesMessageImpl::~JBytesMessageImpl()
{
	JLOG(INFO) << "JBytesMessageImpl::~JBytesMessageImpl, " << length;
}

void JBytesMessageImpl::SetBytes(unsigned char * data, int length)
{
	if(length <= 0 || data == nullptr)
	{
		this->length = 0;
		this->data = nullptr;
	}
	else
	{
		if(this->data != nullptr)
		{
			this->data.release();
		}
		this->length = length;
		this->data = make_unique<unsigned char[]>(length);
		memcpy(this->data.get(), data, length);
	}
}
int JBytesMessageImpl::GetLength()
{
	return length;
}
int JBytesMessageImpl::GetBytes(unsigned char * data, int length)
{
	if(length <= 0 || data == nullptr)
	{
		return -1;
	}
	if(this->data == nullptr)
	{
		return 0;
	}
	if(length > this->length)
	{
		memcpy(data, this->data.get(), this->length);
		return this->length;
	}
	memcpy(data, this->data.get(), length);
	return length;
}

uint8_t * JBytesMessageImpl::GetBytes()
{
	if(this->data == nullptr)
	{
		return nullptr;
	}
	return this->data.get();
}

void JBytesMessageImpl::SetDestination(string destination)
{
	this->destination = destination;
}

string JBytesMessageImpl::GetDestination()
{
	return this->destination;
}

void JBytesMessageImpl::SetSource(string source)
{
	this->source = source;
}

string JBytesMessageImpl::GetSource()
{
	return this->source;
}

