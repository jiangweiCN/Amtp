package amtpsa_interface

import "unsafe"

//const JWUMQ_UNKNOWN_MESSAGE int = 0
const JWUMQ_TEXT_MESSAGE int = 1
const JWUMQ_BYTES_MESSAGE int = 2

const (
	AmtpMsgReserve				= 0x00
	AmtpMsgLogin					= 0x00010001
	AmtpMsgLoginResp				= 0x00020001
	AmtpMsgLogout					= 0x00030001
	AmtpMsgLogoutResp				= 0x00040001
	AmtpMsgConfig					= 0x00050001
	AmtpMsgConfigResp				= 0x00060001
	AmtpMsgReportEvent			= 0x00070001
	AmtpMsgReportEventResp		= 0x00080001
	AmtpMsgAlarm					= 0x00090001
	AmtpMsgAlarmResp				= 0x000A0001
	AmtpMsgAlarmClear				= 0x000B0001
	AmtpMsgAlarmClearResp		= 0x000C0001
	AmtpMsgGpsInfo				= 0x000D0001
	AmtpMsgGpsInfoResp			= 0x000E0001
	AmtpMsgStatusInfo				= 0x000F0001
	AmtpMsgStatusInfoResp		= 0x00100001
	AmtpMsgConfigData				= 0x00110001
	AmtpMsgConfigDataResp		= 0x00120001
	AmtpMsgConfigNotify			= 0x00130001
	AmtpMsgConfigNotifyResp		= 0x00140001
	AmtpMsgUploadFile				= 0x00150001
	AmtpMsgUploadFileResp		= 0x00160001
	AmtpMsgUploadEof				= 0x00170001
	AmtpMsgUploadEofResp			= 0x00180001
	AmtpMsgRestartCmd				= 0x00190001
	AmtpMsgRestartCmdResp		= 0x001A0001
	AmtpMsgQueryData				= 0x001B0001
	AmtpMsgQueryDataResp			= 0x001C0001
	AmtpMsgUploadFileData		= 0x001D0001
	AmtpMsgUploadFileDataResp	= 0x001E0001
)
const (
	Public = 0
	Private = 1
)


type AmtpsaObj interface {
	LibVersion() string
	Connect() int
	SetLimit(limit int) int
	Send(msg interface{}) int
	Receive(timeout int) interface{}
	CreateBytesMessage(cmd uint32, desId string, data []byte) interface{}
}

type JwumqBytesMessageObj interface {
	GetPointer() unsafe.Pointer
	SetBytes(data []byte, length int)
	GetBytes() []byte
	GetLength() int
	SetDestination(destination string)
	GetDestination() string
	GetSource() string
	GetCmd() uint32
}



