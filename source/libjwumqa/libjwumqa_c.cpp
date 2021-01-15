//
//  libjwumqa_py.cpp
//  Amtp
//
//  Created by jiangwei on 2020/6/30.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#include <stdio.h>
#include "libjwumqa_c.h"
#include "libjwumqa_cpp.h"



const char * jwumq_version_c()
{
	return libjwumqa_version;
}

void * jwumq_create_factory_c(char * url)
{
	return new JwumqFactory(url);
}

void jwumq_destroy_factory_c(void * factory)
{
	JwumqFactory* p = (JwumqFactory*)factory;
	if(p != NULL)
	{
		delete p;
	}
}

void * jwumq_create_connection_c(void * factory)
{
	JwumqFactory* p = (JwumqFactory*)factory;
	if(p == NULL)
	{
		return NULL;
	}
	return p->CreateConnection();
}

void jwumq_destroy_connection_c(void * factory, void * connection)
{
	JwumqFactory* p = (JwumqFactory*)factory;
	if(p != NULL && connection != NULL)
	{
		p->DestroyConnection((JwumqConnection *)connection);
	}
}

int jwumq_connection_c(void * connection)
{
	JwumqConnection* p = (JwumqConnection*)connection;
	if(p == NULL)
	{
		return JWUMQ_PARA_IS_NULL;
	}
	return p->Connect();
}

void jwumq_close_c(void * connection)
{
	JwumqConnection* p = (JwumqConnection*)connection;
	if(p == NULL)
	{
		return;
	}
	p->Close();
}


void * jwumq_create_session_c(void * connection)
{
	JwumqConnection* p = (JwumqConnection*)connection;
	if(p == NULL)
	{
		return NULL;
	}
	return p->CreateSession();
}
void jwumq_destroy_session_c(void * connection, void * session)
{
	JwumqConnection* p = (JwumqConnection*)connection;
	if(session != NULL && connection != NULL)
	{
		p->DestroySession((JwumqSession*)session);
	}
}

void * jwumq_create_destination_c(void * session, char * queue_name)
{
	JwumqSession* p = (JwumqSession*)session;
	if(session == NULL || queue_name == NULL)
	{
		return NULL;
	}
	return p->CreateDestination(queue_name);
}

void jwumq_destroy_destination_c(void * session, void * destination)
{
	JwumqSession* p = (JwumqSession*)session;
	if(p != NULL && destination != NULL)
	{
		p->DestroyDestination((JwumqDestination*)destination);
	}
}

void * jwumq_create_part_pipe_c(void * destination, char * node_id)
{
	JwumqDestination* p = (JwumqDestination*)destination;
	if(destination == NULL || node_id == NULL)
	{
		return NULL;
	}
	return p->CreatePartPipe(node_id);
}
void * jwumq_create_part_member_c(void * destination, char * node_id)
{
	JwumqDestination* p = (JwumqDestination*)destination;
	if(destination == NULL || node_id == NULL)
	{
		return NULL;
	}
	return p->CreatePartMember(node_id);
}
void jwumq_destroy_part_c(void * destination, void * part)
{
	JwumqDestination* p = (JwumqDestination*)destination;
	if(p != NULL && part != NULL)
	{
		p->DestroyPart((JwumqPart*)part);
	}
}

void * jwumq_create_text_message_c(void * session, char * text)
{
	JwumqSession* p = (JwumqSession*)session;
	if(session == NULL || text == NULL)
	{
		return NULL;
	}
	return p->CreateTextMessage(text);
}

void * jwumq_create_bytes_message_c(void * session, unsigned char * bytes, int length)
{
	JwumqSession* p = (JwumqSession*)session;
	if(session == NULL || bytes == NULL || length <= 0)
	{
		return NULL;
	}
	return p->CreateBytesMessage(bytes, length);
}

void jwumq_destroy_message_c(void * session, void * message)
{
	JwumqSession* p = (JwumqSession*)session;
	if(p != NULL && message != NULL)
	{
		p->DestroyMessage((JMessage*)message);
	}
}

int jwumq_send_c(void * part, void * message)
{
	JwumqPart* p = (JwumqPart*)part;
	if(part == NULL || message == NULL)
	{
		return JWUMQ_PARA_IS_NULL;
	}
	return p->Send((JMessage *)message);
}

void * jwumq_receive_c(void * part, int timeout, int *type, int *msg_len)
{
	JwumqPart* p = (JwumqPart*)part;
	if(part == NULL)
	{
		return NULL;
	}
	JMessage * message = p->Receive(timeout);
	if(dynamic_cast<JTextMessage*>(message))
	{
		JTextMessage* msg = (JTextMessage*)message;
		*type = JWUMQ_TEXT_MESSAGE;
		*msg_len = msg->GetLength();
	}
	else if(dynamic_cast<JBytesMessage*>(message))
	{
		JBytesMessage* msg = (JBytesMessage*)message;
		*type = JWUMQ_BYTES_MESSAGE;
		*msg_len = msg->GetLength();
	}
	else
	{
		*type = JWUMQ_UNKNOWN_MESSAGE;
		*msg_len = 0;
	}
	return message;
}

char * jwumq_get_text_message_c(void * message)
{
	JMessage * msg = (JMessage*)message;
	if(dynamic_cast<JTextMessage*>(msg))
	{
		JTextMessage* m = (JTextMessage*)message;
		return (char*)m->GetText().data();
	}
	return NULL;
}

int jwumq_get_bytes_message_c(void * message, unsigned char *data, int length)
{
	JMessage * msg = (JMessage*)message;
	if(dynamic_cast<JBytesMessage*>(msg))
	{
		JBytesMessage* m = (JBytesMessage*)message;
		m->GetBytes(data, length);
		return length;
	}
	return 0;
}

void jwumq_message_set_destination_c(void * message, char *des)
{
	JMessage * msg = (JMessage*)message;
	if(dynamic_cast<JTextMessage*>(msg))
	{
		JTextMessage* m = (JTextMessage*)message;
		m->SetDestination(des);
	}
	else if(dynamic_cast<JBytesMessage*>(msg))
	{
		JBytesMessage* m = (JBytesMessage*)message;
		m->SetDestination(des);
	}
}
char * jwumq_get_message_destination_c(void * message)
{
	JMessage * msg = (JMessage*)message;
	if(dynamic_cast<JTextMessage*>(msg))
	{
		JTextMessage* m = (JTextMessage*)message;
		return (char*)m->GetDestination().data();
	}
	else if(dynamic_cast<JBytesMessage*>(msg))
	{
		JBytesMessage* m = (JBytesMessage*)message;
		return (char*)m->GetDestination().data();
	}
	return NULL;
}
char * jwumq_get_message_source_c(void * message)
{
	JMessage * msg = (JMessage*)message;
	if(dynamic_cast<JTextMessage*>(msg) || dynamic_cast<JBytesMessage*>(msg))
	{
		return (char*)msg->GetSource().data();
	}
	return NULL;
	
}
