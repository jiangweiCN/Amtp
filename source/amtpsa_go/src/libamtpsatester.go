package main

import (
	"./amtpsa_interface"
	"container/list"
	"fmt"
	"os"
	"plugin"
	"./amtpap_msg"
	"github.com/golang/protobuf/proto"
	"sync"
	"time"
)

type CacheMessage struct {
	data []byte
	source string
}
func main() {
	fmt.Println("******************************libamtpsa_tester begin!!!******************************")

	if len(os.Args) != 4 {
		panic("para error")
	}
	var node_id = os.Args[1]
	var cmd_address = os.Args[2]
	var data_address = os.Args[3]

	libPlugin, err := plugin.Open("libamtpsa_plugin.so")
	if err != nil {
		panic(err)
	}

	newAmtpsa, err := libPlugin.Lookup("NewAmtpsa")
	if err != nil {
		panic(err)
	}

	newAmtpsaFun, ok := newAmtpsa.(func(string, string, string) interface{})
	if ok == false {
		panic("func type error: NewAmtpsa.")
	}
	
	//NewAmtpsa 返回空接口,把空接口转换成`AmtpsaObj`接口
	amtpsa, ok := newAmtpsaFun(node_id, cmd_address, data_address).(amtpsa_interface.AmtpsaObj)
	if ok == false {
		panic("Type error: AmtpsaObj.")
	}
	//调用接口的方法
	libamtpsaVersion := amtpsa.LibVersion()
	fmt.Println(libamtpsaVersion)
	connectRes := amtpsa.Connect()
	fmt.Println("amtpsa connect result: ", connectRes)
	setRes := amtpsa.SetLimit(200)
	fmt.Println("amtpsa set limit result: ", setRes)

	cacheList := list.New()
	var mutex sync.Mutex
	ch := make(chan struct{})
	count := 2 // count 表示活动的协程个数
	go func() {
		//time.Sleep(3 * time.Second)
		for {
			recvMsg := amtpsa.Receive(5000)
			if recvMsg == nil{
				fmt.Println("amtpsa recv time out.")
				continue
			}

			switch t := recvMsg.(type) {
			case amtpsa_interface.JwumqBytesMessageObj:
				msg := recvMsg.(amtpsa_interface.JwumqBytesMessageObj)
				cacheMsg := CacheMessage{data:msg.GetBytes(), source:msg.GetSource()}
				mutex.Lock()
				cacheList.PushBack(cacheMsg)
				fmt.Println("amtpsa recv msg and push into cache.")
				mutex.Unlock()
				break
			default:
				_ = t
				fmt.Println("Recv unknown message")
				break
			}
		}
		ch <- struct{}{} // 协程结束，发出信号
	}()
	go func() {
		//time.Sleep(3 * time.Second)
		for {
			if cacheList.Len() <= 0{
				time.Sleep(10 * time.Millisecond)
				continue
			}
			mutex.Lock()
			cacheMsg := cacheList.Front()
			if cacheMsg != nil{
				cacheList.Remove(cacheMsg)
			}
			mutex.Unlock()

			if cacheMsg == nil{
				time.Sleep(10 * time.Millisecond)
				continue
			}
			ParseMsg(amtpsa, cacheMsg.Value.(CacheMessage))
		}
		ch <- struct{}{} // 协程结束，发出信号
	}()

	for range ch {
		count--
		if count == 0 {
			close(ch)
		}
	}
	fmt.Println("******************************libamtpsa_tester end!******************************")
}
func ParseMsg(amtpsa amtpsa_interface.AmtpsaObj, msg CacheMessage) {
	protocol_msg:=&amtpap_msg.CmdPrimitive{}
	err := proto.Unmarshal(msg.data, protocol_msg)
	if err != nil {
		fmt.Println("unmarshal err : ", err)
	}

	fmt.Println("amtpsa recv message from: ", msg.source)
	if protocol_msg.GetMsgId() == amtpap_msg.MsgID_LOGIN_V1{
		fmt.Println("recv MsgID: ", protocol_msg.GetMsgId(), ", MsgType:", protocol_msg.GetType())

		tokenString := "test token"
		token := []byte(tokenString)
		var loginRespMsg = &amtpap_msg.LoginRespV1{
			Token: token,
			ConfVersion: 101,
			Result: 1,
		}

		loginRespMsgMsgBytes, err := proto.Marshal(loginRespMsg)
		if err != nil {
			fmt.Println("marshaling err : ", err)
			return
		}

		var sendMsg = &amtpap_msg.CmdPrimitive{
			Type: amtpap_msg.MsgType_PUBLIC,
			SerialNumber: 1,
			MsgId: amtpap_msg.MsgID_LOGIN_RESP_V1,
			Direction: false,
			PayloadData:loginRespMsgMsgBytes,

		}
		sendMsgBytes, err1 := proto.Marshal(sendMsg)
		if err1 != nil {
			fmt.Println("marshaling err : ", err1)
			return
		}
		msg := amtpsa.CreateBytesMessage(amtpsa_interface.AmtpMsgLoginResp, msg.source, sendMsgBytes).(amtpsa_interface.JwumqBytesMessageObj)
		fmt.Println("Send login resp result: ", amtpsa.Send(msg))
	}
}
