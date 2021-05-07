package main

import (
	"./jwumqa_interface"
	"../../amtpsa_go/src/amtpsa_interface"
	"fmt"
	"os"
	"plugin"
	"strconv"
	"time"
)

func main() {
	fmt.Println("******************************libjwumqa_tester begin!!!******************************")
	if len(os.Args) < 4 {
		panic("para error")
	}
	var url = ""
	var queueName = ""
	var nodeId = ""
	var destination = ""
	if len(os.Args) == 4 {
		url = os.Args[1]
		queueName := os.Args[2]
		nodeId := os.Args[3]
		fmt.Println("url: ", url, " , queue name: ", queueName, " , node id: ", nodeId)
	}
	if len(os.Args) == 5 {
		url = os.Args[1]
		queueName = os.Args[2]
		nodeId = os.Args[3]
		destination = os.Args[4]
		fmt.Println("url: ", url, " , queue name: ", queueName, " , node id: ", nodeId, " , destination id: ", destination)
	}

	libPlugin, err := plugin.Open("libjwumqa_plugin.so")
	if err != nil {
		panic(err)
	}

	pluginVersion, err := libPlugin.Lookup("Version")
	if err != nil {
		panic(err)
	}
	pluginVersion.(func())()

	newJwumqFactory, err := libPlugin.Lookup("NewJwumqFactory")
	if err != nil {
		panic(err)
	}
	newJwumqFactoryFun, ok := newJwumqFactory.(func(string) interface{})
	if ok == false {
		panic("func type error: NewJwumqFactory.")
	}
	//NewJwumqFactory 返回空接口,把空接口转换成`JwumqFactoryObj`接口
	jwumqFactory, ok := newJwumqFactoryFun(url).(jwumqa_interface.JwumqFactoryObj)
	if ok == false {
		panic("Type error: JwumqFactoryObj.")
	}
	//调用接口的方法
	libjwumqaVersion := jwumqFactory.LibVersion()
	fmt.Println(libjwumqaVersion)
	factoryInfo := jwumqFactory.FactoryInfo()
	fmt.Println(factoryInfo)

	connection := jwumqFactory.CreateConnection().(jwumqa_interface.JwumqConnectionObj)
	res := connection.Connect()
	fmt.Println("Connect result: ", res)

	session := connection.CreateSession().(jwumqa_interface.JwumqSessionObj)
	dest := session.CreateDestination(queueName).(jwumqa_interface.JwumqDestinationObj)
	//pipe := dest.CreatePartPipe(nodeId).(jwumqa_interface.JwumqPartPipeObj)

	//ch := make(chan struct{})
	//count := 2 // count 表示活动的协程个数
	//go func() {
	//	time.Sleep(3 * time.Second)
	//	for i := 0; i < 3; i++ {
	//		text := nodeId + "_" + strconv.Itoa(i)
	//		textMsg := session.CreateTextMessage(text).(jwumqa_interface.JwumqTextMessageObj)
	//		pipe.Send(textMsg.GetPointer())
	//		session.DestroyMessage(textMsg.GetPointer())
	//		time.Sleep(1 * time.Second)
	//
	//		text += "_bytes"
	//		var data = []byte(text)
	//		bytesMsg := session.CreateBytesMessage(data).(jwumqa_interface.JwumqBytesMessageObj)
	//		pipe.Send(bytesMsg.GetPointer())
	//		session.DestroyMessage(bytesMsg.GetPointer())
	//		time.Sleep(1 * time.Second)
	//	}
	//	ch <- struct{}{} // 协程结束，发出信号
	//}()
	//go func() {
	//	//time.Sleep(3 * time.Second)
	//	for i := 0; i < 6; i++ {
	//		recvMsg := pipe.Receive(5000)
	//		switch t := recvMsg.(type) {
	//		case jwumqa_interface.JwumqTextMessageObj:
	//			msg := recvMsg.(jwumqa_interface.JwumqTextMessageObj)
	//			fmt.Println("RecvThread recv text message: ", msg.GetText())
	//			break
	//		case jwumqa_interface.JwumqBytesMessageObj:
	//			msg := recvMsg.(jwumqa_interface.JwumqBytesMessageObj)
	//			data := msg.GetBytes()
	//			var str string = string(data[:])
	//			fmt.Println("RecvThread recv bytes message, ", str)
	//			break
	//		default:
	//			_ = t
	//			fmt.Println("RecvThread recv unknown message")
	//			break
	//		}
	//	}
	//	ch <- struct{}{} // 协程结束，发出信号
	//}()


	member := dest.CreatePartMember(nodeId).(jwumqa_interface.JwumqPartMemberObj)
	ch := make(chan struct{})
	count := 2 // count 表示活动的协程个数
	go func() {
		time.Sleep(3 * time.Second)
		for i := 0; i < 10; i++ {
			text := nodeId + "_" + strconv.Itoa(i)
			textMsg := session.CreateTextMessage(text).(jwumqa_interface.JwumqTextMessageObj)
			textMsg.SetDestination(destination)
			member.Send(textMsg.GetPointer())
			session.DestroyMessage(textMsg.GetPointer())
			time.Sleep(1 * time.Second)

			text += "_bytes"
			var data = []byte(text)
			bytesMsg := session.CreateBytesMessage(data).(jwumqa_interface.JwumqBytesMessageObj)
			bytesMsg.SetDestination(destination)
			member.Send(bytesMsg.GetPointer())
			session.DestroyMessage(bytesMsg.GetPointer())
			time.Sleep(1 * time.Second)
		}
		ch <- struct{}{} // 协程结束，发出信号
	}()
	go func() {
		//time.Sleep(3 * time.Second)
		for i := 0; i < 20; i++ {
			recvMsg := member.Receive(5000)
			switch t := recvMsg.(type) {
			case jwumqa_interface.JwumqTextMessageObj:
				msg := recvMsg.(jwumqa_interface.JwumqTextMessageObj)
				fmt.Println("RecvThread recv text message, source:", msg.GetSource(), ", des:", msg.GetDestination(), ", text: ", msg.GetText())
				break
			case jwumqa_interface.JwumqBytesMessageObj:
				msg := recvMsg.(jwumqa_interface.JwumqBytesMessageObj)
				data := msg.GetBytes()
				var str string = string(data[:])
				fmt.Println("RecvThread recv bytes message, source:", msg.GetSource(), ", des:", msg.GetDestination(), ", text: ", str)
				break
			default:
				_ = t
				fmt.Println("RecvThread recv unknown message")
				break
			}
		}
		ch <- struct{}{} // 协程结束，发出信号
	}()

	for range ch {
		count--
		if count == 0 {
			close(ch)
		}
	}

	ch1 := make(chan struct{})
	count = 1
	go func() {
		fmt.Println("--------------------Load amtpsa------------------")
		libPlugin_s, err := plugin.Open("libamtpsa_plugin.so")
		if err != nil {
			panic(err)
		}

		newAmtpsa, err := libPlugin_s.Lookup("NewAmtpsa")
		if err != nil {
			panic(err)
		}

		newAmtpsaFun, ok := newAmtpsa.(func(string, string, string) interface{})
		if ok == false {
			panic("func type error: NewAmtpsa.")
		}
		
		//NewAmtpsa 返回空接口,把空接口转换成`AmtpsaObj`接口
		amtpsa, ok := newAmtpsaFun("node_id", "tcp://127.0.0.1:5581", "tcp://127.0.0.1:5580").(amtpsa_interface.AmtpsaObj)
		if ok == false {
			panic("Type error: AmtpsaObj.")
		}
		//调用接口的方法
		libamtpsaVersion := amtpsa.LibVersion()
		fmt.Println(libamtpsaVersion)
		fmt.Println("--------------------Load amtpsa------------------")
		ch1 <- struct{}{} // 协程结束，发出信号
	}()

	for range ch1 {
		count--
		if count == 0 {
			close(ch1)
		}
	}
	

	dest.DestroyPart(member.GetPointer())
	//dest.DestroyPart(pipe.GetPointer())
	session.DestroyDestination(dest.GetPointer())
	connection.DestroySession(session.GetPointer())
	connection.Close()
	jwumqFactory.DestroyConnection(connection.GetPointer())
	//fmt.Println(reflect.TypeOf(connection).String())
	jwumqFactory.DestroyFactory()

	fmt.Println("******************************libjwumqa_tester end!******************************")
}

//func SendThread(s string, ch chan string) {
//	for i := 0; i < 5; i++ {
//		time.Sleep(1 * time.Second)
//		fmt.Println("SendThread send ", i+1)
//	}
//	ch <- "SendThread exit!"
//	close(ch)
//}
//
//func RecvThread(s string, ch chan string) {
//	for i := 0; i < 5; i++ {
//		time.Sleep(1 * time.Second)
//		fmt.Println("RecvThread recv ", i+1)
//	}
//	ch <- "SendThread exit!"
//	close(ch)
//}
