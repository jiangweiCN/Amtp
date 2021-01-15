//
//  libjwumqa_cpp.h
//  Amtp
//
//  Created by jiangwei on 2020/6/17.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_h
#define libjwumqa_h

#include <stdio.h>
#include <string>
#include <memory>


using namespace std;


#define JWUMQ_SUCCESS				0
#define JWUMQ_SEND_TIMEOUT			-201
#define JWUMQ_POOL_ACCESS_FAILED	-202
#define JWUMQ_MESSAGE_IS_NULL		-203
#define JWUMQ_MESSAGE_UNKNOWN		-204
#define JWUMQ_PARA_IS_NULL			-204

#define JWUMQ_UNKNOWN_MESSAGE		0
#define JWUMQ_TEXT_MESSAGE			1
#define JWUMQ_BYTES_MESSAGE			2

constexpr auto libjwumqa_version = "libjwumqa version 2.0.1.20200702";

class JMessage
{
public:
	virtual ~JMessage(void){};
	virtual void SetSource(string source) = 0;
	virtual string GetSource() = 0;
	virtual void SetDestination(string destination) = 0;
	virtual string GetDestination() = 0;
protected:
	string destination;
	string source;
};

class JTextMessage:public JMessage
{
public:
	virtual ~JTextMessage(void){};
	
	virtual void SetText(string text) = 0;
	virtual string GetText() = 0;
	virtual int GetLength() = 0;
};

class JBytesMessage:public JMessage
{
public:
	virtual ~JBytesMessage(void){};
	
	virtual void SetBytes(uint8_t * data, int length) = 0;
	virtual int GetLength() = 0;
	virtual int GetBytes(uint8_t * data, int length) = 0;
	virtual uint8_t * GetBytes() = 0;
};

class JwumqConnection;
class JwumqFactory
{
public:
	JwumqFactory();
	JwumqFactory(string url);
	~JwumqFactory(void);
	
	void SetUrl(string url);
	JwumqConnection* CreateConnection();
	void DestroyConnection(JwumqConnection*);
private:
	string factory_url;
};

class JwumqSession;
class JwumqConnection
{
public:
	virtual ~JwumqConnection(void){};
	
	virtual int Connect() = 0;
	virtual void Close() = 0;
	virtual JwumqSession* CreateSession() = 0;
	virtual void DestroySession(JwumqSession*) = 0;
};

class JwumqDestination;
class JwumqSession
{
public:
	virtual ~JwumqSession(void){};
	
	virtual JwumqDestination* CreateDestination(string queue_name) = 0;
	virtual JTextMessage* CreateTextMessage(string text) = 0;
	virtual JTextMessage* CreateTextMessage() = 0;
	virtual JBytesMessage* CreateBytesMessage(unsigned char * data, int length) = 0;
	virtual JBytesMessage* CreateBytesMessage() = 0;
	virtual void DestroyDestination(JwumqDestination*) = 0;
	virtual void DestroyMessage(JMessage * message) = 0;
};

class JwumqPart;
class JwumqPartPipe;
class JwumqPartMember;
class JwumqPartProducer;
class JwumqPartConsumer;
class JwumqDestination
{
public:
	virtual ~JwumqDestination(void){};
	
	virtual JwumqPartPipe* CreatePartPipe(string node_id) = 0;
	virtual JwumqPartMember* CreatePartMember(string node_id) = 0;
	virtual JwumqPartProducer* CreatePartProducer(string node_id) = 0;
	virtual JwumqPartConsumer* CreatePartConsumer(string node_id) = 0;
	virtual void DestroyPart(JwumqPart*) = 0;
};

class JwumqPart
{
public:
	virtual ~JwumqPart(void){};
	virtual int Send(JMessage *) = 0;
	virtual JMessage * Receive(int timeout) = 0;
};

class JwumqPartPipe :public JwumqPart
{
public:
	virtual ~JwumqPartPipe(void){};
	
	virtual int Send(JMessage *) = 0;
	virtual JMessage * Receive(int timeout) = 0;
};

class JwumqPartMember :public JwumqPart
{
public:
	virtual ~JwumqPartMember(void){};
	
	virtual int Send(JMessage *) = 0;
	virtual JMessage * Receive(int timeout) = 0;
};

class JwumqPartProducer :public JwumqPart
{
public:
	virtual ~JwumqPartProducer(void){};
	
	virtual int Send(JMessage *) = 0;
	virtual JMessage * Receive(int timeout) = 0;
};

class JwumqPartConsumer :public JwumqPart
{
public:
	virtual ~JwumqPartConsumer(void){};
	
	virtual int Send(JMessage *) = 0;
	virtual JMessage * Receive(int timeout) = 0;
};


#endif /* libjwumqa_h */
