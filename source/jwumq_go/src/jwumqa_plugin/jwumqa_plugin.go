package main
/*
#cgo LDFLAGS: -L .  -lstdc++ -ldl
#cgo CFLAGS: -I ./
#cgo CXXFLAGS: -std=c++14
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>


typedef char* (*LIB_JWUMQ_VERSION_C)();
typedef void* (*LIB_JWUMQ_CREATE_FACTORY_C)(char *);
typedef void (*LIB_JWUMQ_DESTROY_FACTORY_C)(void *);
typedef void* (*LIB_JWUMQ_CREATE_CONNECTION_C)(void *);
typedef void (*LIB_JWUMQ_DESTROY_CONNECTION_C)(void *, void *);
typedef int (*LIB_JWUMQ_CONNECT_C)(void *);
typedef void (*LIB_JWUMQ_CLOSE_C)(void *);
typedef void* (*LIB_JWUMQ_CREATE_SESSION_C)(void *);
typedef void (*LIB_JWUMQ_DESTROY_SESSION_C)(void *, void *);
typedef void* (*LIB_JWUMQ_CREATE_DESTINATION_C)(void *, char *);
typedef void (*LIB_JWUMQ_DESTROY_DESTINATION_C)(void *, void *);
typedef void* (*LIB_JWUMQ_CREATE_PART_PIPE_C)(void *, char *);
typedef void* (*LIB_JWUMQ_CREATE_PART_MEMBER_C)(void *, char *);
typedef void (*LIB_JWUMQ_DESTROY_PART_C)(void *, void *);
typedef void* (*LIB_JWUMQ_CREATE_TEXT_MESSAGE_C)(void *, char *);
typedef void* (*LIB_JWUMQ_CREATE_BYTES_MESSAGE_C)(void *, unsigned char *, int);
typedef void (*LIB_JWUMQ_DESTROY_MESSAGE_C)(void *, void *);
typedef int (*LIB_JWUMQ_SEND_C)(void *, void *);
typedef void* (*LIB_JWUMQ_RECEIVE_C)(void *, int, int *, int *);
typedef char* (*LIB_JWUMQ_GET_TEXT_MESSAGE_C)(void *);
typedef int (*LIB_JWUMQ_GET_BYTES_MESSAGE_C)(void *, unsigned char *, int);
typedef void (*LIB_JWUMQ_MESSAGE_SET_DESTINATION_C)(void *, char *);
typedef char* (*LIB_JWUMQ_MESSAGE_DESTINATION_C)(void *);
typedef char* (*LIB_JWUMQ_MESSAGE_SOURCE_C)(void *);


LIB_JWUMQ_VERSION_C version = NULL;
LIB_JWUMQ_CREATE_FACTORY_C jwumq_create_factory = NULL;
LIB_JWUMQ_DESTROY_FACTORY_C jwumq_destroy_factory = NULL;
LIB_JWUMQ_CREATE_CONNECTION_C jwumq_create_connection = NULL;
LIB_JWUMQ_DESTROY_CONNECTION_C jwumq_destroy_connection = NULL;
LIB_JWUMQ_CONNECT_C jwumq_connect = NULL;
LIB_JWUMQ_CLOSE_C jwumq_close = NULL;
LIB_JWUMQ_CREATE_SESSION_C jwumq_create_session = NULL;
LIB_JWUMQ_DESTROY_SESSION_C jwumq_destroy_session = NULL;
LIB_JWUMQ_CREATE_DESTINATION_C jwumq_create_destination = NULL;
LIB_JWUMQ_DESTROY_DESTINATION_C jwumq_destroy_destination = NULL;
LIB_JWUMQ_CREATE_PART_PIPE_C jwumq_create_part_pipe = NULL;
LIB_JWUMQ_CREATE_PART_MEMBER_C jwumq_create_part_member = NULL;
LIB_JWUMQ_DESTROY_PART_C jwumq_destroy_part = NULL;
LIB_JWUMQ_CREATE_TEXT_MESSAGE_C jwumq_create_text_message = NULL;
LIB_JWUMQ_CREATE_BYTES_MESSAGE_C jwumq_create_bytes_message = NULL;
LIB_JWUMQ_DESTROY_MESSAGE_C jwumq_destroy_message = NULL;
LIB_JWUMQ_SEND_C jwumq_send = NULL;
LIB_JWUMQ_RECEIVE_C jwumq_receive = NULL;
LIB_JWUMQ_GET_TEXT_MESSAGE_C jwumq_get_text_message = NULL;
LIB_JWUMQ_GET_BYTES_MESSAGE_C jwumq_get_bytes_message = NULL;
LIB_JWUMQ_MESSAGE_SET_DESTINATION_C jwumq_message_set_destination = NULL;
LIB_JWUMQ_MESSAGE_DESTINATION_C jwumq_message_destination = NULL;
LIB_JWUMQ_MESSAGE_SOURCE_C jwumq_message_source = NULL;

void* LibLoad(char* so_file)
{
    void* lib_handle;
	lib_handle = dlopen(so_file, RTLD_NOW);
	if (lib_handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray %s error:%s\n", so_file, dlerror());
		return NULL;
	}
	version = (LIB_JWUMQ_VERSION_C)dlsym(lib_handle, "jwumq_version_c");
	jwumq_create_factory = (LIB_JWUMQ_CREATE_FACTORY_C)dlsym(lib_handle, "jwumq_create_factory_c");
	jwumq_destroy_factory = (LIB_JWUMQ_DESTROY_FACTORY_C)dlsym(lib_handle, "jwumq_destroy_factory_c");
	jwumq_create_connection = (LIB_JWUMQ_CREATE_CONNECTION_C)dlsym(lib_handle, "jwumq_create_connection_c");
	jwumq_destroy_connection = (LIB_JWUMQ_DESTROY_CONNECTION_C)dlsym(lib_handle, "jwumq_destroy_connection_c");
	jwumq_connect = (LIB_JWUMQ_CONNECT_C)dlsym(lib_handle, "jwumq_connection_c");
	jwumq_close = (LIB_JWUMQ_CLOSE_C)dlsym(lib_handle, "jwumq_close_c");
	jwumq_create_session = (LIB_JWUMQ_CREATE_SESSION_C)dlsym(lib_handle, "jwumq_create_session_c");
	jwumq_destroy_session = (LIB_JWUMQ_DESTROY_SESSION_C)dlsym(lib_handle, "jwumq_destroy_session_c");
	jwumq_create_destination = (LIB_JWUMQ_CREATE_DESTINATION_C)dlsym(lib_handle, "jwumq_create_destination_c");
	jwumq_destroy_destination = (LIB_JWUMQ_DESTROY_DESTINATION_C)dlsym(lib_handle, "jwumq_destroy_destination_c");
	jwumq_create_part_pipe = (LIB_JWUMQ_CREATE_PART_PIPE_C)dlsym(lib_handle, "jwumq_create_part_pipe_c");
	jwumq_create_part_member = (LIB_JWUMQ_CREATE_PART_MEMBER_C)dlsym(lib_handle, "jwumq_create_part_member_c");
	jwumq_destroy_part = (LIB_JWUMQ_DESTROY_PART_C)dlsym(lib_handle, "jwumq_destroy_part_c");
	jwumq_create_text_message = (LIB_JWUMQ_CREATE_TEXT_MESSAGE_C)dlsym(lib_handle, "jwumq_create_text_message_c");
	jwumq_create_bytes_message = (LIB_JWUMQ_CREATE_BYTES_MESSAGE_C)dlsym(lib_handle, "jwumq_create_bytes_message_c");
	jwumq_destroy_message = (LIB_JWUMQ_DESTROY_MESSAGE_C)dlsym(lib_handle, "jwumq_destroy_message_c");
	jwumq_send = (LIB_JWUMQ_SEND_C)dlsym(lib_handle, "jwumq_send_c");
	jwumq_receive = (LIB_JWUMQ_RECEIVE_C)dlsym(lib_handle, "jwumq_receive_c");
	jwumq_get_text_message = (LIB_JWUMQ_GET_TEXT_MESSAGE_C)dlsym(lib_handle, "jwumq_get_text_message_c");
	jwumq_get_bytes_message = (LIB_JWUMQ_GET_BYTES_MESSAGE_C)dlsym(lib_handle, "jwumq_get_bytes_message_c");
	jwumq_message_set_destination = (LIB_JWUMQ_MESSAGE_SET_DESTINATION_C)dlsym(lib_handle, "jwumq_message_set_destination_c");
	jwumq_message_destination = (LIB_JWUMQ_MESSAGE_DESTINATION_C)dlsym(lib_handle, "jwumq_get_message_destination_c");
	jwumq_message_source = (LIB_JWUMQ_MESSAGE_SOURCE_C)dlsym(lib_handle, "jwumq_get_message_source_c");

	if(version == NULL || jwumq_create_factory == NULL || jwumq_destroy_factory == NULL
 			|| jwumq_create_connection == NULL || jwumq_destroy_connection == NULL
 			|| jwumq_connect == NULL || jwumq_close == NULL
 			|| jwumq_create_destination == NULL || jwumq_destroy_destination == NULL
 			|| jwumq_create_part_pipe == NULL || jwumq_create_part_member == NULL || jwumq_destroy_part == NULL
 			|| jwumq_create_text_message == NULL || jwumq_create_bytes_message == NULL || jwumq_destroy_message == NULL
 			|| jwumq_send == NULL || jwumq_receive == NULL
 			|| jwumq_get_text_message == NULL || jwumq_get_bytes_message == NULL
			|| jwumq_message_set_destination == NULL || jwumq_message_destination == NULL || jwumq_message_source == NULL)
	{
		return NULL;
	}

    return lib_handle;
}
char * Dlerror(void)
{
	return dlerror();
}
char * JwumqVersion()
{
	return version();
}
void * JwumqCreateFactory(char * url)
{
	return jwumq_create_factory(url);
}
void JwumqDestroyFactory(void * factory)
{
	jwumq_destroy_factory(factory);
}
void * JwumqCreateConnection(void * factory)
{
	return jwumq_create_connection(factory);
}
void JwumqDestroyConnection(void * factory, void * connection)
{
	jwumq_destroy_connection(factory, connection);
}
int JwumqConnect(void * connection)
{
	return jwumq_connect(connection);
}
void JwumqClose(void * connection)
{
	jwumq_close(connection);
}
void * JwumqCreateSession(void * connection)
{
	return jwumq_create_session(connection);
}
void JwumqDestroySession(void * connection, void * session)
{
	jwumq_destroy_session(connection, session);
}
void * JwumqCreateDestination(void * session, char * queue_name)
{
	return jwumq_create_destination(session, queue_name);
}
void JwumqDestroyDestination(void * session, void * destination)
{
	jwumq_destroy_destination(session, destination);
}
void * JwumqCreatePartPipe(void * destination, char * node_id)
{
	return jwumq_create_part_pipe(destination, node_id);
}
void * JwumqCreatePartMember(void * destination, char * node_id)
{
	return jwumq_create_part_member(destination, node_id);
}
void JwumqDestroyPart(void * destination, void * part)
{
	jwumq_destroy_part(destination, part);
}
void *JwumqCreateTextMessage(void * session, char * text)
{
	return jwumq_create_text_message(session, text);
}
void * JwumqCreateBytesMessage(void * session, void * bytes, int length)
{
	return jwumq_create_bytes_message(session, (unsigned char*)bytes, length);
}
void JwumqDestroyMessage(void * session, void * message)
{
	jwumq_destroy_message(session, message);
}
int JwumqSend(void * part, void * message)
{
	return jwumq_send(part, message);
}
void * JwumqReceive(void * part, int timeout, int *type, int *msg_len)
{
	return jwumq_receive(part, timeout, type, msg_len);
}
char * JwumqGetTextMessage(void * message)
{
	int len = strlen(jwumq_get_text_message(message));
	if(len <= 0)
	{
		fprintf(stderr, "JwumqGetTextMessage error, length of msg text <= 0\n");
		return NULL;
	}
	char* text = malloc(len + 1);
	strcpy(text, jwumq_get_text_message(message));
	text[len] = 0;
	// fprintf(stderr, "JwumqGetTextMessage:%s, %s\n", jwumq_get_text_message(message), text);
	return text;

	// fprintf(stderr, "JwumqGetTextMessage:%s, %d\n", jwumq_get_text_message(message), strlen(jwumq_get_text_message(message)));
	// return jwumq_get_text_message(message);
}
int JwumqGetBytesMessage(void * message, void *data, int length)
{
	return jwumq_get_bytes_message(message, (unsigned char*)data, length);
}
int JwumqMessageSetDestination(void * message, char *des)
{
	jwumq_message_set_destination(message, des);
}
// char * JwumqMessageDestination(void * message)
// {
// 	fprintf(stderr, "JwumqMessageDestination:%s, %d\n", jwumq_message_destination(message), strlen(jwumq_message_destination(message)));
// 	return jwumq_message_destination(message);
// }
char * JwumqMessageDestination(void * message)
{
	int len = strlen(jwumq_message_destination(message));
	if(len <= 0)
	{
		fprintf(stderr, "JwumqMessageDestination error, length of destination id <= 0\n");
		return NULL;
	}
	char* des = malloc(len + 1);
	strcpy(des, jwumq_message_destination(message));
	des[len] = 0;
	// fprintf(stderr, "JwumqMessageDestination_e:%s, %s\n", jwumq_message_destination(message), des);
	return des;
}
char * JwumqMessageSource(void * message)
{
	int len = strlen(jwumq_message_source(message));
	if(len <= 0)
	{
		fprintf(stderr, "JwumqMessageSource error, length of source id <= 0\n");
		return NULL;
	}
	char* src = malloc(len + 1);
	strcpy(src, jwumq_message_source(message));
	src[len] = 0;
	// fprintf(stderr, "JwumqMessageSource:%s, %s\n", jwumq_message_source(message), des);
	return src;
	// fprintf(stderr, "JwumqMessageSource:%s, %d\n", jwumq_message_source(message), strlen(jwumq_message_source(message)));
	// return jwumq_message_source(message);
}
*/
import "C"
import (
	"fmt"
	"../jwumqa_interface"
	"unsafe"
)

func init() {
	fmt.Println("jwumqa_plugin load............")
	//我们还可以做其他更高阶的事情，比如 platform.RegisterPlugin({"func": Hello}) 之类的，向插件平台自动注册该插件的函数
}

func Version() {
	fmt.Println("jwumqa_plugin version 1.0.1.20200706170842")
}

//////////////////////////////////////////////// JwumqFactory 定义
type JwumqFactory struct {
	//内部封装数据
	Url string
	version string
	libHandle unsafe.Pointer
	pointer unsafe.Pointer
}
///////////////////////////////////////////////////////////////////////////////////////JwumqConnection 定义
type JwumqConnection struct {
	factory *JwumqFactory
	pointer unsafe.Pointer
}
///////////////////////////////////////////////////////////////////////////////////////JwumqSession 定义
type JwumqSession struct {
	conn *JwumqConnection
	pointer unsafe.Pointer
}
///////////////////////////////////////////////////////////////////////////////////////JwumqSession 定义
type JwumqDestination struct {
	session *JwumqSession
	pointer unsafe.Pointer
	queueName string
}
///////////////////////////////////////////////////////////////////////////////////////JwumqPartPipe 定义
type JwumqPartPipe struct {
	destination *JwumqDestination
	pointer unsafe.Pointer
	nodeID string
}
///////////////////////////////////////////////////////////////////////////////////////JwumqPartMember 定义
type JwumqPartMember struct {
	destination *JwumqDestination
	pointer unsafe.Pointer
	nodeID string
}
///////////////////////////////////////////////////////////////////////////////////////JwumqTextMessage 定义
type JwumqTextMessage struct {
	text string
	pointer unsafe.Pointer
	destination string
	source string
}
///////////////////////////////////////////////////////////////////////////////////////JwumqBytesMessage 定义
type JwumqBytesMessage struct {
	data []byte
	length int
	pointer unsafe.Pointer
	destination string
	source string
}
///////////////////////////////////////////////////////////////////JwumqFactory 方法的实现
func (p *JwumqFactory) LibVersion() string {
	return fmt.Sprintf("libjwumqa.so version: %s", p.version)
}
func (p *JwumqFactory) FactoryInfo() string {
	return fmt.Sprintf("Jwumq_factory info: %s", p.Url)
}
func (p *JwumqFactory) DestroyFactory()  {
	C.JwumqDestroyFactory(p.pointer)
}
func (p *JwumqFactory) CreateConnection() interface{}{
	connection := JwumqConnection{factory:p}
	connection.pointer = C.JwumqCreateConnection(connection.factory.pointer)
	return &connection
}
func (p *JwumqFactory) DestroyConnection(pointer unsafe.Pointer)  {
	C.JwumqDestroyConnection(p.pointer, pointer)
}
///////////////////////////////////////////////////////////////////JwumqConnection 方法的实现
func (p *JwumqConnection) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqConnection) Connect() int{
	return int(C.JwumqConnect(p.pointer))
}
func (p *JwumqConnection) Close(){
	C.JwumqClose(p.pointer)
}
func (p *JwumqConnection) CreateSession() interface{}{
	session := JwumqSession{conn:p}
	session.pointer = C.JwumqCreateSession(session.conn.pointer)
	return &session
}
func (p *JwumqConnection) DestroySession(pointer unsafe.Pointer)  {
	C.JwumqDestroySession(p.pointer, pointer)
}
///////////////////////////////////////////////////////////////////JwumqSession 方法的实现
func (p *JwumqSession) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqSession) CreateDestination(queueName string) interface{}{
	dest := JwumqDestination{session:p}
	dest.queueName = queueName
	cstr := C.CString(queueName)
	dest.pointer = C.JwumqCreateDestination(dest.session.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
	return &dest
}
func (p *JwumqSession) DestroyDestination(pointer unsafe.Pointer)  {
	C.JwumqDestroyDestination(p.pointer, pointer)
}
func (p *JwumqSession) CreateTextMessage(text string) interface{}{
	msg := JwumqTextMessage{text:text}
	cstr := C.CString(text)
	msg.pointer = C.JwumqCreateTextMessage(p.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
	return &msg
}
func (p *JwumqSession) CreateBytesMessage(data []byte) interface{}{
	msg := JwumqBytesMessage{data:data}
	msg.pointer = C.JwumqCreateBytesMessage(p.pointer, unsafe.Pointer(&data[0]), C.int(len(data)))
	return &msg
}
func (p *JwumqSession) DestroyMessage(pointer unsafe.Pointer)  {
	C.JwumqDestroyMessage(p.pointer, pointer)
}
///////////////////////////////////////////////////////////////////JwumqDestination 方法的实现
func (p *JwumqDestination) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqDestination) CreatePartPipe(nodeID string) interface{}{
	part := JwumqPartPipe{destination:p}
	part.nodeID = nodeID
	cstr := C.CString(nodeID)
	part.pointer = C.JwumqCreatePartPipe(part.destination.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
	return &part
}
func (p *JwumqDestination) CreatePartMember(nodeID string) interface{}{
	member := JwumqPartMember{destination:p}
	member.nodeID = nodeID
	cstr := C.CString(nodeID)
	member.pointer = C.JwumqCreatePartMember(member.destination.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
	return &member
}
func (p *JwumqDestination) DestroyPart(pointer unsafe.Pointer)  {
	C.JwumqDestroyPart(p.pointer, pointer)
}
///////////////////////////////////////////////////////////////////JwumqPartPipe 方法的实现
func (p *JwumqPartPipe) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqPartPipe) Send(pointer unsafe.Pointer) int{
	return int(C.JwumqSend(p.pointer, pointer))
}
func (p *JwumqPartPipe) Receive(timeout int) interface{}{
	var msgType int
	var msgLength int
	recv_msg := C.JwumqReceive(p.pointer, C.int(timeout), (*C.int)(unsafe.Pointer(&msgType)), (*C.int)(unsafe.Pointer(&msgLength)))
	if msgType == jwumqa_interface.JWUMQ_TEXT_MESSAGE{
		// text := C.GoString(C.JwumqGetTextMessage(recv_msg))
		text_str := C.JwumqGetTextMessage(recv_msg)
		text := C.GoString(text_str)
		C.free(unsafe.Pointer(text_str))

		// destination := C.GoString(C.JwumqMessageDestination(msg))
		destination_str := C.JwumqMessageDestination(recv_msg)
		destination := C.GoString(destination_str)
		C.free(unsafe.Pointer(destination_str))

		// source := C.GoString(C.JwumqMessageSource(msg))
		source_str := C.JwumqMessageSource(recv_msg)
		source := C.GoString(source_str)
		C.free(unsafe.Pointer(source_str))

		msg := JwumqTextMessage{text:text}
		msg.destination = destination
		msg.source = source

		C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
		return &msg
	}else if msgType == jwumqa_interface.JWUMQ_BYTES_MESSAGE{
		data := make([]byte, msgLength)
		res := int(C.JwumqGetBytesMessage(recv_msg, unsafe.Pointer(&data[0]), C.int(msgLength)))
		// destination := C.GoString(C.JwumqMessageDestination(msg))
		destination_str := C.JwumqMessageDestination(recv_msg)
		destination := C.GoString(destination_str)
		C.free(unsafe.Pointer(destination_str))

		// source := C.GoString(C.JwumqMessageSource(msg))
		source_str := C.JwumqMessageSource(recv_msg)
		source := C.GoString(source_str)
		C.free(unsafe.Pointer(source_str))

		if res <= 0{
			return nil
		}
		msg := JwumqBytesMessage{data:data}
		msg.destination = destination
		msg.source = source

		C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
		return &msg
	}
	C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
	return nil
}
///////////////////////////////////////////////////////////////////JwumqPartMember 方法的实现
func (p *JwumqPartMember) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqPartMember) Send(pointer unsafe.Pointer) int{
	return int(C.JwumqSend(p.pointer, pointer))
}
func (p *JwumqPartMember) Receive(timeout int) interface{}{
	var msgType int
	var msgLength int
	recv_msg := C.JwumqReceive(p.pointer, C.int(timeout), (*C.int)(unsafe.Pointer(&msgType)), (*C.int)(unsafe.Pointer(&msgLength)))
	if msgType == jwumqa_interface.JWUMQ_TEXT_MESSAGE{
		// text := C.GoString(C.JwumqGetTextMessage(recv_msg))
		text_str := C.JwumqGetTextMessage(recv_msg)
		text := C.GoString(text_str)
		C.free(unsafe.Pointer(text_str))
		
		// destination := C.GoString(C.JwumqMessageDestination(recv_msg))
		destination_str := C.JwumqMessageDestination(recv_msg)
		destination := C.GoString(destination_str)
		C.free(unsafe.Pointer(destination_str))

		// source := C.GoString(C.JwumqMessageSource(recv_msg))
		source_str := C.JwumqMessageSource(recv_msg)
		source := C.GoString(source_str)
		C.free(unsafe.Pointer(source_str))

		// fmt.Println("plugin, src:", source, ", destination:", destination, ", d2:", d2, ", text:", text)
		msg := JwumqTextMessage{text:text}
		msg.destination = destination
		msg.source = source
		// fmt.Println("plugin, src:", msg.source, ", destination:", msg.destination, ", text:", msg.text)
		C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
		return &msg
	}else if msgType == jwumqa_interface.JWUMQ_BYTES_MESSAGE{
		data := make([]byte, msgLength)
		res := int(C.JwumqGetBytesMessage(recv_msg, unsafe.Pointer(&data[0]), C.int(msgLength)))
		if res <= 0{
			return nil
		}
		destination_str := C.JwumqMessageDestination(recv_msg)
		destination := C.GoString(destination_str)
		C.free(unsafe.Pointer(destination_str))

		// source := C.GoString(C.JwumqMessageSource(recv_msg))
		source_str := C.JwumqMessageSource(recv_msg)
		source := C.GoString(source_str)
		C.free(unsafe.Pointer(source_str))

		// fmt.Println("plugin, src:", source, ", destination:", destination, ", d2:", d2)
		msg := JwumqBytesMessage{data:data}
		msg.destination = destination
		msg.source = source
		// fmt.Println("plugin, src:", msg.source, ", destination:", msg.destination)
		C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
		return &msg
	}
	C.JwumqDestroyMessage(p.destination.session.pointer, recv_msg)
	return nil
}
///////////////////////////////////////////////////////////////////JwumqTextMessage 方法的实现
func (p *JwumqTextMessage) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqTextMessage) SetText(text string){
	p.text = text
}
func (p *JwumqTextMessage) GetText() string{
	return p.text
}
func (p *JwumqTextMessage) SetDestination(destination string) {
	p.destination = destination
	cstr := C.CString(destination)
	C.JwumqMessageSetDestination(p.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
}
func (p *JwumqTextMessage) GetDestination() string {
	return p.destination
}
func (p *JwumqTextMessage) GetSource() string{
	return p.source
}
///////////////////////////////////////////////////////////////////JwumqBytesMessage 方法的实现
func (p *JwumqBytesMessage) GetPointer() unsafe.Pointer{
	return p.pointer
}
func (p *JwumqBytesMessage) SetBytes(data []byte, length int){
	p.data = data
	p.length = length
}
func (p *JwumqBytesMessage) GetBytes() []byte{
	return p.data
}
func (p *JwumqBytesMessage) GetLength() int{
	return p.length
}
func (p *JwumqBytesMessage) SetDestination(destination string) {
	p.destination = destination
	cstr := C.CString(destination)
	C.JwumqMessageSetDestination(p.pointer, cstr)
	defer C.free(unsafe.Pointer(cstr))
}
func (p *JwumqBytesMessage) GetDestination() string {
	return p.destination
}
func (p *JwumqBytesMessage) GetSource() string{
	return p.source
}
//////////////////////////////返回函数
func NewJwumqFactory(url string) interface{}{
	handle := C.LibLoad(C.CString("libjwumqa.so"))
	if handle == nil {
		fmt.Println("dlopen error, ", C.GoString(C.Dlerror()))
		return nil
	}else{
		fmt.Println("libjwumqa.so dlopen success!")
	}
	factory := JwumqFactory{Url:url, version:C.GoString(C.JwumqVersion()), libHandle:handle}
	factory.pointer = C.JwumqCreateFactory(C.CString(factory.Url))
	return &factory
}










