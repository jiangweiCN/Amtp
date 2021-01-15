//
//  jwumq_define.h
//  Amtp
//
//  Created by jiangwei on 2020/1/8.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwumq_define_h
#define jwumq_define_h

#include <functional>

constexpr auto MAX_IDENTITY_BUF_SIZE = 64;
constexpr auto MAX_ADDRESS_BUF_SIZE = 64;
constexpr auto MAX_TYPE_BUF_SIZE = 64;

#define LIB_JWUMQ_SUCCESS			0
#define LIB_JWUMQ_IS_WORKING		-1
#define LIB_JWIMQ_NO_WORKING		-2
#define LIB_JWUMQ_PARA_ABNORMAL		-3
#define LIB_JWUMQ_TYPE_ABNORMAL		-4
#define LIB_JWUMQ_FUN_UNAVAILABLE	-5
#define LIB_JWUMQ_SOCKET_FAILD		-6
#define LIB_JWUMQ_CONNECT_FAILD		-7
#define LIB_JWUMQ_BIND_FAILD		-8
#define LIB_JWUMQ_SEND_DATA_FAILD	-9
#define LIB_JWUMQ_SEND_ID_FAILD		-10


enum class JWUMQ_COMMAND_ENUM : uint32_t
{
	public_data = 0x00,
	public_data_ack,
	private_data,
	private_data_ack,
	private_service_start,
	private_service_start_ack,
	private_request_config,
	private_request_config_ack,
	private_update_config,
	private_update_config_ack,
	private_pool_status_request,
	private_pool_status_report,
	private_alive_req,
	private_alive_resp,
	private_session_req,
	private_session_resp,
	private_create_queue_req,
	private_create_queue_resp,
	private_Assigned_queue_req,
	private_create_part_pipe_req,
	private_create_part_pipe_resp,
	private_create_part_member_req,
	private_create_part_member_resp,
	private_create_part_producer_req,
	private_create_part_producer_resp,
	private_create_part_consumer_req,
	private_create_part_consumer_resp,
	public_data_text,
	public_data_bytes,
	public_producer_data_text,
	public_producer_data_bytes,
	public_consumer_data_text,
	public_consumer_data_bytes,
};

enum class JWUMQ_TYPE_ENUM : uint16_t 
{
	router = 0x01,
	proxy,
	dealer,
	duplex_pub,
	duplex_sub,
	router_server,
};


enum class ZMQ_TYPE_ENUM : uint16_t
{
	pair = 0x00,
	pub,
	sub,
	req,
	rep,
	dealer,
	router,
	pull,
	push,
	xpub,
	xsub,
	stream,
	unknown = 0xffff,
};

using JWUMQ_SETUP_CONF_T = struct jwumq_setup_conf_s
{
	JWUMQ_TYPE_ENUM type;
	char identity[MAX_IDENTITY_BUF_SIZE];
};

using ROUTER_CONF_T = struct router_conf_s : jwumq_setup_conf_s
{
	bool frontend_bind;
	bool backend_bind;
	ZMQ_TYPE_ENUM frontend_type;
	ZMQ_TYPE_ENUM backend_type;
	char frontend_addr[MAX_ADDRESS_BUF_SIZE];
	char backend_addr[MAX_ADDRESS_BUF_SIZE];
};

using PROXY_CONF_T = struct proxy_conf_s : jwumq_setup_conf_s
{
	bool frontend_bind;
	bool backend_bind;
	ZMQ_TYPE_ENUM frontend_type;
	ZMQ_TYPE_ENUM backend_type;
	char frontend_addr[MAX_ADDRESS_BUF_SIZE];
	char backend_addr[MAX_ADDRESS_BUF_SIZE];
};

using DEALER_CONF_T = struct dealer_conf_s : jwumq_setup_conf_s
{
	char address[MAX_ADDRESS_BUF_SIZE];
	int linger;
	int read_timeout;
	bool bind;
};

using ROUTER_SERVER_CONF_T = struct router_server_conf_s : jwumq_setup_conf_s
{
	char address[MAX_ADDRESS_BUF_SIZE];
	int linger;
	int read_timeout;
	bool bind;
};

using SOCKET_MONITOR_CONF_T = struct socket_monitor_conf_s : jwumq_setup_conf_s
{
	char address[MAX_ADDRESS_BUF_SIZE];
};

using DUPLEX_PUB_SUB_CONF_T = struct duplex_pub_sub_conf_s : jwumq_setup_conf_s
{
	char pub_addr[MAX_ADDRESS_BUF_SIZE];
	char dealer_addr[MAX_ADDRESS_BUF_SIZE];
};

using JWUMQ_RECV_CALLBACK = std::function<int (void * )>;
typedef int(*JWUMQ_RECV_CALLBACK_C)(int, char *, void * , int);


#endif /* jwumq_define_h */
