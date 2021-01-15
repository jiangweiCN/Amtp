//
//  libjwumqa_c.h
//  Amtp
//
//  Created by jiangwei on 2020/7/6.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef libjwumqa_c_h
#define libjwumqa_c_h


extern "C"
{
	const char * jwumq_version_c();
	void * jwumq_create_factory_c(char * url);
	void jwumq_destroy_factory_c(void * factory);
	void * jwumq_create_connection_c(void * factory);
	void jwumq_destroy_connection_c(void * factory, void * connection);
	int jwumq_connection_c(void * connection);
	void jwumq_close_c(void * connection);
	void * jwumq_create_session_c(void * connection);
	void jwumq_destroy_session_c(void * connection, void * session);
	void * jwumq_create_destination_c(void * session, char * queue_name);
	void jwumq_destroy_destination_c(void * session, void * destination);
	void * jwumq_create_part_pipe_c(void * destination, char * node_id);
	void * jwumq_create_part_member_c(void * destination, char * node_id);
	void jwumq_destroy_part_c(void * destination, void * part);
	void * jwumq_create_text_message_c(void * session, char * text);
	void * jwumq_create_bytes_message_c(void * session, unsigned char * bytes, int length);
	void jwumq_destroy_message_c(void * session, void * message);
	int jwumq_send_c(void * part, void * message);
	void * jwumq_receive_c(void * part, int timeout, int *type, int *msg_len);
	char * jwumq_get_text_message_c(void * message);
	int jwumq_get_bytes_message_c(void * message, unsigned char *data, int length);
	void jwumq_message_set_destination_c(void * message, char *data);
	char * jwumq_get_message_destination_c(void * message);
	char * jwumq_get_message_source_c(void * message);
}

#endif /* libjwumqa_c_h */
