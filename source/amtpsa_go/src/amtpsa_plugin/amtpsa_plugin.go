package main
/*
#cgo LDFLAGS: -L .  -lstdc++ -ldl
#cgo CFLAGS: -I ./
#cgo CXXFLAGS: -std=c++14
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

typedef char* (*LIB_AMTPSA_VERSION_C)();
typedef void* (*LIB_AMTPSA_LOAD_C)();
typedef int (*LIB_AMTPSA_CONNECTION_C)(void *, char * , char *, char * );
typedef int (*LIB_AMTPSA_SETLIMIT_C)(void *, int);
typedef int (*LIB_AMTPSA_SENDMSG_C)(void *, u_int32_t, char * , void * , int );
typedef int (*LIB_AMTPSA_READMSG_C)(void *, void * , void * , int );
typedef int (*LIB_AMTPSA_WAITFORMSG_C)(void *, int *, int);
typedef void* (*LIB_AMTPSA_RELEASE_C)(void *);

LIB_AMTPSA_VERSION_C lib_amtpsa_version = NULL;
LIB_AMTPSA_LOAD_C lib_amtpsa_load = NULL;
LIB_AMTPSA_CONNECTION_C lib_amtpsa_connection = NULL;
LIB_AMTPSA_SETLIMIT_C lib_amtpsa_setlimit = NULL;
LIB_AMTPSA_WAITFORMSG_C lib_amtpsa_waitformsg = NULL;
LIB_AMTPSA_READMSG_C lib_amtpsa_readmsg = NULL;
LIB_AMTPSA_SENDMSG_C lib_amtpsa_sendmsg = NULL;

void* LibamtpsaLoad(char* so_file)
{
    void* lib_handle;
	lib_handle = dlopen(so_file, RTLD_NOW);
	if (lib_handle == NULL)
	{
		fprintf(stderr, "Failed to open libaray %s error:%s\n", so_file, dlerror());
		return NULL;
	}
	lib_amtpsa_version = (LIB_AMTPSA_VERSION_C)dlsym(lib_handle, "amtpsa_version");
	lib_amtpsa_load = (LIB_AMTPSA_LOAD_C)dlsym(lib_handle, "amtpsa_load");
	lib_amtpsa_connection = (LIB_AMTPSA_CONNECTION_C)dlsym(lib_handle, "amtpsa_connection");
	lib_amtpsa_setlimit = (LIB_AMTPSA_SETLIMIT_C)dlsym(lib_handle, "amtpsa_setlimit");
	lib_amtpsa_waitformsg = (LIB_AMTPSA_WAITFORMSG_C)dlsym(lib_handle, "amtpsa_waitformsg");
	lib_amtpsa_readmsg = (LIB_AMTPSA_READMSG_C)dlsym(lib_handle, "amtpsa_readmsg");
	lib_amtpsa_sendmsg = (LIB_AMTPSA_SENDMSG_C)dlsym(lib_handle, "amtpsa_sendmsg");


	if(lib_amtpsa_version == NULL
		|| lib_amtpsa_load == NULL
		|| lib_amtpsa_connection == NULL
		|| lib_amtpsa_setlimit == NULL
		|| lib_amtpsa_readmsg == NULL
		|| lib_amtpsa_sendmsg == NULL)
	{
		return NULL;
	}

    return lib_handle;
}

char * AmtpsaVersion()
{
	return lib_amtpsa_version();
}

void * AmtpsaLoad()
{
	return lib_amtpsa_load();
}

int AmtpsaConnect(void * handle, char* node_id, char* cmd_address, char* data_address)
{
	return lib_amtpsa_connection(handle, node_id, cmd_address, data_address);
}

int AmtpsaSetlimit(void * handle, int limit)
{
	return lib_amtpsa_setlimit(handle, limit);
}

int AmtpsaWaitForMsg(void * handle, int * msg_len, int timeout)
{
	return lib_amtpsa_waitformsg(handle, msg_len, timeout);
}

int AmtpsaReceive(void * handle, void * src_id, void * data, int data_len)
{
	return lib_amtpsa_readmsg(handle, src_id, data, data_len);
}

int AmtpsaSend(void * handle, u_int32_t cmd, char * des_id, void * data, int data_len)
{
	return lib_amtpsa_sendmsg(handle, cmd, des_id, data, data_len);
}

char * Dlerror(void)
{
	return dlerror();
}
*/
import "C"
import (
	"fmt"
	"../amtpsa_interface"
	"unsafe"
)

func init() {
	fmt.Println("amtpsa_plugin load............")
	//我们还可以做其他更高阶的事情，比如 platform.RegisterPlugin({"func": Hello}) 之类的，向插件平台自动注册该插件的函数
}

func Version() {
	fmt.Println("jwumqa_plugin version 1.0.1.20200706170842")
}

//////////////////////////////////////////////// Amtpsa 定义
type Amtpsa struct {
	//内部封装数据
	nodeId string
	cmdAddr string
	dataAddr string
	version string
	libHandle unsafe.Pointer
	pointer unsafe.Pointer
}
///////////////////////////////////////////////////////////////////////////////////////JwumqBytesMessage 定义
type JwumqBytesMessage struct {
	data []byte
	length int
	pointer unsafe.Pointer
	destination string
	source string
	cmd uint32
}
///////////////////////////////////////////////////////////////////Amtpsa 方法的实现
func (p *Amtpsa) LibVersion() string {
	return fmt.Sprintf("libamtpsa.so version: %s", p.version)
}

func (p *Amtpsa) Connect() int{
	//fmt.Println("Amtpsa Connect handle, ", p.pointer)
	return int(C.AmtpsaConnect(p.pointer, C.CString(p.nodeId), C.CString(p.cmdAddr), C.CString(p.dataAddr)))
}

func (p *Amtpsa) SetLimit(limit int) int{
	return int(C.AmtpsaSetlimit(p.pointer, C.int(limit)))
}

func (p *Amtpsa) WaitForCmd(msgLen *int32, timeout int) int{
	return int(C.AmtpsaWaitForMsg(p.pointer, (*C.int)(unsafe.Pointer(msgLen)), C.int(timeout)))
}

func (p *Amtpsa) Receive(timeout int) interface{}{
	var msgLen int32
	result := p.WaitForCmd(&msgLen, timeout)
	//fmt.Println("amtpsa wait for cmd result:", result, ", msg len:", msgLen)
	if msgLen > 0 && result > 0{
		data := make([]byte, msgLen)
		srcId := make([]byte, 64)
		C.AmtpsaReceive(p.pointer, unsafe.Pointer(&srcId[0]), unsafe.Pointer(&data[0]), C.int(msgLen))
		msg := JwumqBytesMessage{data:data}
		msg.source = *(*string)(unsafe.Pointer(&srcId))
		return &msg
	}
	return nil
}

func (p *Amtpsa) CreateBytesMessage(cmd uint32, desId string, data []byte) interface{}{
	msg := JwumqBytesMessage{data:data, cmd:cmd, destination:desId}
	return &msg
}

func (p *Amtpsa) Send(msg interface{}) int{
	sendMsg := msg.(amtpsa_interface.JwumqBytesMessageObj)
	data := sendMsg.GetBytes()
	return int(C.AmtpsaSend(p.pointer, C.uint(sendMsg.GetCmd()), C.CString(sendMsg.GetDestination()), unsafe.Pointer(&data[0]), C.int(len(data))))
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
}
func (p *JwumqBytesMessage) GetDestination() string {
	return p.destination
}
func (p *JwumqBytesMessage) GetSource() string{
	return p.source
}
func (p *JwumqBytesMessage) GetCmd() uint32{
	return p.cmd
}
func (p *JwumqBytesMessage) SetCmd(cmd uint32) {
	p.cmd = cmd
}

////////////////////////////////返回函数
func NewAmtpsa(id string, cmdAddr string, dataAddr string) interface{}{
	handle := C.LibamtpsaLoad(C.CString("./libamtpsa.so"))
	if handle == nil {
		fmt.Println("dlopen error, ", C.GoString(C.Dlerror()))
		return nil
	}else{
		fmt.Println("libamtpsa.so dlopen success!")
	}
	amtpsa := Amtpsa{nodeId:id, cmdAddr:cmdAddr, dataAddr:dataAddr, version:C.GoString(C.AmtpsaVersion()), libHandle:handle}
	amtpsa.pointer = C.AmtpsaLoad()
	fmt.Println("Amtpsa handle, ", amtpsa.pointer)
	return &amtpsa
}









