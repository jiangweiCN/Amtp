//
//  JTextMessageImpl.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/28.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include "JTextMessageImpl.hpp"
#include "../jlog/jLog.h"



JTextMessageImpl::JTextMessageImpl(string text):text(text)
{
	JLOG(INFO) << "JTextMessageImpl::JTextMessageImpl, " << text.c_str();
}

JTextMessageImpl::JTextMessageImpl()
{
	JLOG(INFO) << "JTextMessageImpl::JTextMessageImpl, " << text.c_str();
}

JTextMessageImpl::~JTextMessageImpl()
{
	JLOG(INFO) << "JTextMessageImpl::~JTextMessageImpl, " << text.c_str();
}

void JTextMessageImpl::SetText(string text)
{
	this->text = text;
}

string JTextMessageImpl::GetText()
{
	return text;
}

int JTextMessageImpl::GetLength()
{
	return (int)this->text.length();
}

void JTextMessageImpl::SetSource(string source)
{
	this->source = source;
}

void JTextMessageImpl::SetDestination(string destination)
{
	this->destination = destination;
}

string JTextMessageImpl::GetDestination()
{
	return this->destination;
}

string JTextMessageImpl::GetSource()
{
	return this->source;
}
