//
//  libjwumqa_tester.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/16.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <thread>


#include "../libjwumqa_cpp.h"
#include "../../jwumq/jwumq_message.hpp"

using namespace std;

int mode = 0;

string Time();
void ParseArg(int argc, char *argv[]);
void PrintHelp(const char* prog);


class JwumqPipeTest
{
public:
	JwumqPipeTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory);
	~JwumqPipeTest(void);
	void Start();
private:
	void SendThread(JwumqSession* session, JwumqPartPipe* pipe);
	void RecvThread(JwumqSession* session, JwumqPartPipe* pipe);
private:
	string url;
	string queue_name;
	string node_id;
	shared_ptr<JwumqFactory> factory;
	int loop;

};

JwumqPipeTest::JwumqPipeTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory)
{
	this->url = url;
	this->queue_name = queue_name;
	this->node_id = node_id;
	this->factory = factory;
}

JwumqPipeTest::~JwumqPipeTest()
{
}

void JwumqPipeTest::Start()
{
	JwumqConnection * conn = factory->CreateConnection();
	conn->Connect();
	
	JwumqSession * session = conn->CreateSession();
	if(session == nullptr)
	{
		conn->Close();
		factory->DestroyConnection(conn);
		return ;
	}

	JwumqDestination * destination = session->CreateDestination(queue_name);

	if(destination == nullptr)
	{
		conn->DestroySession(session);
		conn->Close();
		factory->DestroyConnection(conn);
		return;
	}
	JwumqPartPipe * pipe = destination->CreatePartPipe(node_id);
	
	loop = 3;
	thread s_t(&JwumqPipeTest::SendThread, this, session, pipe);
	thread r_t(&JwumqPipeTest::RecvThread, this, session, pipe);
	r_t.join();
	s_t.join();
	
	destination->DestroyPart(pipe);
	session->DestroyDestination(destination);
	conn->DestroySession(session);
	conn->Close();
	factory->DestroyConnection(conn);
}

void JwumqPipeTest::SendThread(JwumqSession* session, JwumqPartPipe* pipe)
{
	int index = 0;

	sleep(3);
	while(loop > 0)
	{
		string text = node_id + "_" + to_string(++index);
//		unique_ptr<JTextMessage> tm(session->CreateTextMessage(text));
		unique_ptr<JTextMessage> tm(session->CreateTextMessage());
		tm->SetText(text);
		pipe->Send(tm.get());
		
		sleep(1);
		
		text += "_bytes";
		unsigned char buf[128] = {0};
		memcpy(buf, text.c_str(), text.length());
		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage((unsigned char *)buf, (int)text.length()));
//		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage());
//		bm->SetBytes(buf, (int)text.length());
		pipe->Send(bm.get());
		sleep(1);
		
		loop --;
	}
	sleep(3);
}

void JwumqPipeTest::RecvThread(JwumqSession* session, JwumqPartPipe* pipe)
{
	while(loop > 0)
	{
		JMessage * message = pipe->Receive(5000);
		if(dynamic_cast<JTextMessage*>(message))
		{
			JTextMessage * tm = (JTextMessage*)message;
			fprintf(stderr, "Recv JTextMessage, text:%s\n", tm->GetText().c_str());
			session->DestroyMessage(tm);
		}
		else if(dynamic_cast<JBytesMessage*>(message))
		{
			JBytesMessage * bm = (JBytesMessage*)message;
			int length = bm->GetLength();
			unique_ptr<char[]> buf(new char[length + 1]());
			memcpy(buf.get(), bm->GetBytes(), length);
			buf[length] = 0;
			fprintf(stderr, "Recv JBytesMessage, data[%d]:%s\n", length, buf.get());
			session->DestroyMessage(bm);
		}
		sleep(1);
	}
}

class JwumqMemberTest
{

public:
	JwumqMemberTest(string url, string queue_name, string node_id, string des_id, shared_ptr<JwumqFactory> factory);
	~JwumqMemberTest(void);
	void Start();
private:
	void SendThread(JwumqSession* session, JwumqPartMember* member);
	void RecvThread(JwumqSession* session, JwumqPartMember* member);
private:
	string url;
	string queue_name;
	string node_id;
	string des_id;
	shared_ptr<JwumqFactory> factory;
	int loop;

};

JwumqMemberTest::JwumqMemberTest(string url, string queue_name, string node_id, string des_id, shared_ptr<JwumqFactory> factory)
{
	this->url = url;
	this->queue_name = queue_name;
	this->node_id = node_id;
	this->des_id = des_id;
	this->factory = factory;
}

JwumqMemberTest::~JwumqMemberTest()
{
}

void JwumqMemberTest::Start()
{
	JwumqConnection * conn = factory->CreateConnection();
	conn->Connect();
	
	JwumqSession * session = conn->CreateSession();
	if(session == nullptr)
	{
		conn->Close();
		factory->DestroyConnection(conn);
		return ;
	}

	JwumqDestination * destination = session->CreateDestination(queue_name);

	if(destination == nullptr)
	{
		conn->DestroySession(session);
		conn->Close();
		factory->DestroyConnection(conn);
		return;
	}
	JwumqPartMember * member = destination->CreatePartMember(node_id);
	
	if(member != nullptr)
	{
		loop = 3;
		thread s_t(&JwumqMemberTest::SendThread, this, session, member);
		thread r_t(&JwumqMemberTest::RecvThread, this, session, member);
		r_t.join();
		s_t.join();
	}
	
	destination->DestroyPart(member);
	session->DestroyDestination(destination);
	conn->DestroySession(session);
	conn->Close();
	factory->DestroyConnection(conn);
}

void JwumqMemberTest::SendThread(JwumqSession* session, JwumqPartMember* member)
{
	int index = 0;

	sleep(3);
	while(loop > 0)
	{
		string text = node_id + "_" + to_string(++index);
		unique_ptr<JTextMessage> tm(session->CreateTextMessage(text));
		tm->SetDestination(des_id);
		member->Send(tm.get());
		sleep(1);
		
		text += "_bytes";
		unsigned char buf[128] = {0};
		memcpy(buf, text.c_str(), text.length());
		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage((unsigned char *)buf, (int)text.length()));
		bm->SetDestination(des_id);
		member->Send(bm.get());
		sleep(1);
		
		loop --;
	}
	sleep(3);
}

void JwumqMemberTest::RecvThread(JwumqSession* session, JwumqPartMember* member)
{
	fprintf(stderr, "JwumqMemberTest::RecvThread\n");
	while(loop > 0)
	{
		JMessage * message = member->Receive(5000);
		if(dynamic_cast<JTextMessage*>(message))
		{
			JTextMessage * tm = (JTextMessage*)message;
			fprintf(stderr, "Recv JTextMessage, src:%s, des:%s, text:%s\n", message->GetSource().c_str(), message->GetDestination().c_str(), tm->GetText().c_str());
			session->DestroyMessage(tm);
		}
		else if(dynamic_cast<JBytesMessage*>(message))
		{
			JBytesMessage * bm = (JBytesMessage*)message;
			int length = bm->GetLength();
			unique_ptr<char[]> buf(new char[length + 1]());
			memcpy(buf.get(), bm->GetBytes(), length);
			buf[length] = 0;
			fprintf(stderr, "Recv JBytesMessage, src:%s, des:%s, data[%d]:%s\n", message->GetSource().c_str(), message->GetDestination().c_str(), length, buf.get());
			session->DestroyMessage(bm);
		}
		else
		{
			fprintf(stderr, "Recv timeout\n");
		}
//		sleep(1);
	}
}

class JwumqProducerTest
{
public:
	JwumqProducerTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory);
	~JwumqProducerTest(void);
	void Start();
private:
	void SendThread(JwumqSession* session, JwumqPartProducer* producer);
	void RecvThread(JwumqSession* session, JwumqPartProducer* producer);
private:
	string url;
	string queue_name;
	string node_id;
	shared_ptr<JwumqFactory> factory;
	int loop;

};

JwumqProducerTest::JwumqProducerTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory)
{
	this->url = url;
	this->queue_name = queue_name;
	this->node_id = node_id;
	this->factory = factory;
}

JwumqProducerTest::~JwumqProducerTest()
{
}

void JwumqProducerTest::Start()
{
	JwumqConnection * conn = factory->CreateConnection();
	conn->Connect();
	
	JwumqSession * session = conn->CreateSession();
	if(session == nullptr)
	{
		conn->Close();
		factory->DestroyConnection(conn);
		return ;
	}

	JwumqDestination * destination = session->CreateDestination(queue_name);

	if(destination == nullptr)
	{
		conn->DestroySession(session);
		conn->Close();
		factory->DestroyConnection(conn);
		return;
	}
	JwumqPartProducer * producer = destination->CreatePartProducer(node_id);
	
	loop = 3;
	thread s_t(&JwumqProducerTest::SendThread, this, session, producer);
	thread r_t(&JwumqProducerTest::RecvThread, this, session, producer);
	r_t.join();
	s_t.join();
	
	destination->DestroyPart(producer);
	session->DestroyDestination(destination);
	conn->DestroySession(session);
	conn->Close();
	factory->DestroyConnection(conn);
}

void JwumqProducerTest::SendThread(JwumqSession* session, JwumqPartProducer* producer)
{
	int index = 0;

	sleep(3);
	while(loop > 0)
	{
		string text = node_id + "_" + to_string(++index);
		unique_ptr<JTextMessage> tm(session->CreateTextMessage());
		tm->SetText(text);
		producer->Send(tm.get());
		
		sleep(1);
		
		text += "_bytes";
		unsigned char buf[128] = {0};
		memcpy(buf, text.c_str(), text.length());
		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage((unsigned char *)buf, (int)text.length()));
		producer->Send(bm.get());
		sleep(1);
		
		loop --;
	}
	sleep(3);
}

void JwumqProducerTest::RecvThread(JwumqSession* session, JwumqPartProducer* producer)
{
	while(loop > 0)
	{
		JMessage * message = producer->Receive(5000);
		if(dynamic_cast<JTextMessage*>(message))
		{
			JTextMessage * tm = (JTextMessage*)message;
			fprintf(stderr, "Recv JTextMessage, text:%s\n", tm->GetText().c_str());
			session->DestroyMessage(tm);
		}
		else if(dynamic_cast<JBytesMessage*>(message))
		{
			JBytesMessage * bm = (JBytesMessage*)message;
			int length = bm->GetLength();
			unique_ptr<char[]> buf(new char[length + 1]());
			memcpy(buf.get(), bm->GetBytes(), length);
			buf[length] = 0;
			fprintf(stderr, "Recv JBytesMessage, data[%d]:%s\n", length, buf.get());
			session->DestroyMessage(bm);
		}
		sleep(1);
	}
}

class JwumqConsumerTest
{
public:
	JwumqConsumerTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory);
	~JwumqConsumerTest(void);
	void Start();
private:
	void SendThread(JwumqSession* session, JwumqPartConsumer* consumer);
	void RecvThread(JwumqSession* session, JwumqPartConsumer* consumer);
private:
	string url;
	string queue_name;
	string node_id;
	shared_ptr<JwumqFactory> factory;
	int loop;

};

JwumqConsumerTest::JwumqConsumerTest(string url, string queue_name, string node_id, shared_ptr<JwumqFactory> factory)
{
	this->url = url;
	this->queue_name = queue_name;
	this->node_id = node_id;
	this->factory = factory;
}

JwumqConsumerTest::~JwumqConsumerTest()
{
}

void JwumqConsumerTest::Start()
{
	JwumqConnection * conn = factory->CreateConnection();
	conn->Connect();
	
	JwumqSession * session = conn->CreateSession();
	if(session == nullptr)
	{
		conn->Close();
		factory->DestroyConnection(conn);
		return ;
	}

	JwumqDestination * destination = session->CreateDestination(queue_name);

	if(destination == nullptr)
	{
		conn->DestroySession(session);
		conn->Close();
		factory->DestroyConnection(conn);
		return;
	}
	JwumqPartConsumer * consumer = destination->CreatePartConsumer(node_id);
	
	loop = 3;
	thread s_t(&JwumqConsumerTest::SendThread, this, session, consumer);
	thread r_t(&JwumqConsumerTest::RecvThread, this, session, consumer);
	r_t.join();
	s_t.join();
	
	destination->DestroyPart(consumer);
	session->DestroyDestination(destination);
	conn->DestroySession(session);
	conn->Close();
	factory->DestroyConnection(conn);
}

void JwumqConsumerTest::SendThread(JwumqSession* session, JwumqPartConsumer* consumer)
{
	int index = 0;

	sleep(3);
	while(loop > 0)
	{
		string text = node_id + "_" + to_string(++index);
//		unique_ptr<JTextMessage> tm(session->CreateTextMessage(text));
		unique_ptr<JTextMessage> tm(session->CreateTextMessage());
		tm->SetText(text);
		consumer->Send(tm.get());
		
		sleep(1);
		
		text += "_bytes";
		unsigned char buf[128] = {0};
		memcpy(buf, text.c_str(), text.length());
		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage((unsigned char *)buf, (int)text.length()));
//		unique_ptr<JBytesMessage> bm(session->CreateBytesMessage());
//		bm->SetBytes(buf, (int)text.length());
		consumer->Send(bm.get());
		sleep(1);
		
		loop --;
	}
	sleep(3);
}

void JwumqConsumerTest::RecvThread(JwumqSession* session, JwumqPartConsumer* consumer)
{
	while(loop > 0)
	{
		JMessage * message = consumer->Receive(5000);
		if(dynamic_cast<JTextMessage*>(message))
		{
			JTextMessage * tm = (JTextMessage*)message;
			fprintf(stderr, "Recv JTextMessage, text:%s\n", tm->GetText().c_str());
			session->DestroyMessage(tm);
		}
		else if(dynamic_cast<JBytesMessage*>(message))
		{
			JBytesMessage * bm = (JBytesMessage*)message;
			int length = bm->GetLength();
			unique_ptr<char[]> buf(new char[length + 1]());
			memcpy(buf.get(), bm->GetBytes(), length);
			buf[length] = 0;
			fprintf(stderr, "Recv JBytesMessage, data[%d]:%s\n", length, buf.get());
			session->DestroyMessage(bm);
		}
		sleep(1);
	}
}


int main(int argc, char * argv[])
{
	ParseArg(argc, argv);
	
	string proc_name = argv[0];
	string url = argv[2];
	string queue_name = argv[3];
	string node_id = argv[4];
	
	fprintf(stderr, "%s:****************************libjwumqa tester begin****************************\n", Time().c_str());

	shared_ptr<JwumqFactory> factory(new JwumqFactory(url));

	if(mode == 1)
	{
		fprintf(stderr, "Part pipe test, url:%s, queue name:%s, self id:%s\n", url.c_str(), queue_name.c_str(), node_id.c_str());
		JwumqPipeTest test(url, queue_name, node_id, factory);
		test.Start();
	}
	else if (mode == 2)
	{
		string des = "";
		if(argc == 6)
		{
			des = argv[5];
		}
		
		fprintf(stderr, "Part member test, url:%s, queue name:%s, self id:%s, destination:%s\n", url.c_str(), queue_name.c_str(), node_id.c_str(), des.c_str());
		JwumqMemberTest test(url, queue_name, node_id, des, factory);
		test.Start();
	}
	else if(mode == 3)
	{
		fprintf(stderr, "Part producer test, url:%s, queue name:%s, self id:%s\n", url.c_str(), queue_name.c_str(), node_id.c_str());
		JwumqProducerTest test(url, queue_name, node_id, factory);
		test.Start();
	}
	else if(mode == 4)
	{
		fprintf(stderr, "Part consumer test, url:%s, queue name:%s, self id:%s\n", url.c_str(), queue_name.c_str(), node_id.c_str());
		JwumqConsumerTest test(url, queue_name, node_id, factory);
		test.Start();
	}
//	while(true)
//	{
//		sleep(1);
//	}
	fprintf(stderr, "%s:****************************libjwumqa tester end****************************\n", Time().c_str());
	return 0;
}

string Time()
{
	struct timeval tv;
	char buf[32];
	char time_buf[64];
	 
	gettimeofday(&tv, NULL);
	strftime(buf, sizeof(buf)-1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
	sprintf(time_buf, "%s.%03d", buf, (int)(tv.tv_usec / 1000));
	return time_buf;
}

void PrintHelp(const char* prog)
{
	printf("%s usage.\n", prog);
	printf("-p: for test juwmq pipe model\n");
	printf("-m: for test juwmq member model\n");
	printf("-a: for test juwmq producer model\n");
	printf("-b: for test juwmq consumer model\n");
	
//	printf("eg:%s -model proc_id\n", prog);
//	printf("%s -p ptp_001\n", prog);
//	printf("%s -b pub_002\n", prog);
}

void ParseArg(int argc, char *argv[])
{
	int ch = 0;
	if (argc < 4)
	{
		PrintHelp(argv[0]);
		exit(0);
	}
	
	while ((ch = getopt(argc, argv, "p:m:a:b:")) != -1)
	{
		switch (ch)
		{
			case 'p':
				mode = 1;
				std::cout << "Hello jwumq tester, begin pipe test!\n";
				break;
			case 'm':
				mode = 2;
				std::cout << "Hello jwumq tester, begin member test!\n";
			break;
			case 'a':
				mode = 3;
				std::cout << "Hello jwumq tester, begin producer test!\n";
			break;
			case 'b':
				mode = 4;
				std::cout << "Hello jwumq tester, begin consumer test!\n";
			break;
			case '?':
			default:
				PrintHelp(argv[0]);
				exit(0);
				break;
		}
	}
}





