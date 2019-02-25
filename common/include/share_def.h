#pragma once

/************************************************************************/
/* 

*/
/************************************************************************/
#include "PrimitiveDef.h"
/************************************************************************/
/*
tcp
:
alldatalen + '|' + EPackType + data
JSONPACK(1byte) + MainClassify + '|' + AssClassify + '|' +  jsondatalen(str) + '|' + json data
PBPACK(1byte) + next all data len(str) + '|' + protobufmessagename + '|' + protobufmessage
*/
/************************************************************************/

/************************************************************************/
/* 
websocket
:
EPackType + data
JSONPACK(1byte) + MainClassify + AssClassify + json data
PBPACK(1byte) + protobufmessagename + '|' + protobufmessage
*/
/************************************************************************/
enum class EPackType  //
{
	JSONPACK,	//json
	PBPACK,		//protobuf
};


enum class MainClassify //
{
	UndefinedMainCmdL,
	HallLogic=1,
	RoomLogic,
	UndefinedMainCmdH,
};

enum class UndefineAssist
{
	UndefinedAssCmd,
};

enum class HallAssistRequest  //
{
	UndefinedAssCmd,
	UserLogin=1,
	GetAllRooms,
};

enum class HallAssistResponse
{
	UndefinedAssCmd,
	UserLoginResult,
	AllRoomsInfo,
};

enum class HallResponseStatusCode //大厅登陆返回码
{
	LoginRequestErrorFormat,	//大厅登陆数据格式错误
	LoginHallWrongPassword,		//大厅登陆密码错误
	LoginHallForbiden,			//被禁止大厅登陆
	AllRoomsInfoErrorFormat,	//获取房间信息请求格式错误
	AllRoomsInfoNotLoginHanll,	//想获取房间信息但是未登陆
};

enum class RoomAssistRequest //main1的辅助命令类
{
	UndefinedAssCmd,
	EnterDesk = 1,
	LeaveDesk,
	SendTextMessage,
	SendEmojiMessage,
};

enum class RoomAssistResponse //main1的辅助命令类
{
	UndefinedAssCmd,
	EnterDeskResult,
	LeaveDeskResult,
	SendTextMessageResult,
	SendEmojiMessageResult,
};

