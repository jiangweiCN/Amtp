package jwumqa_interface

import "unsafe"

//const JWUMQ_UNKNOWN_MESSAGE int = 0
const JWUMQ_TEXT_MESSAGE int = 1
const JWUMQ_BYTES_MESSAGE int = 2


type JwumqFactoryObj interface {
	LibVersion() string
	FactoryInfo() string
	DestroyFactory()
	CreateConnection() interface{}
	DestroyConnection(p unsafe.Pointer)
}

type JwumqConnectionObj interface {
	GetPointer() unsafe.Pointer
	Connect() int
	Close()
	CreateSession() interface{}
	DestroySession(p unsafe.Pointer)
}

type JwumqSessionObj interface {
	GetPointer() unsafe.Pointer
	CreateDestination(queueName string) interface{}
	DestroyDestination(p unsafe.Pointer)
	CreateTextMessage(text string) interface{}
	CreateBytesMessage(data []byte) interface{}
	DestroyMessage(p unsafe.Pointer)
}

type JwumqDestinationObj interface {
	GetPointer() unsafe.Pointer
	CreatePartPipe(nodeID string) interface{}
	CreatePartMember(nodeID string) interface{}
	DestroyPart(p unsafe.Pointer)
}

type JwumqPartPipeObj interface {
	GetPointer() unsafe.Pointer
	Send(p unsafe.Pointer) int
	Receive(timeout int) interface{}
}

type JwumqPartMemberObj interface {
	GetPointer() unsafe.Pointer
	Send(p unsafe.Pointer) int
	Receive(timeout int) interface{}
}

type JwumqTextMessageObj interface {
	GetPointer() unsafe.Pointer
	SetText(text string)
	GetText() string
	SetDestination(destination string)
	GetDestination() string
	GetSource() string
}

type JwumqBytesMessageObj interface {
	GetPointer() unsafe.Pointer
	SetBytes(data []byte, length int)
	GetBytes() []byte
	GetLength() int
	SetDestination(destination string)
	GetDestination() string
	GetSource() string
}

