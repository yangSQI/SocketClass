#ifndef __DATAGATHER_H__
#define __DATAGATHER_H__
enum class DataType
{
	LOGIN,
	LOGOUT,
	LOGIN_REPLY,
	LOGOUT_REPLY
};
struct DataHeader
{
	int len;
	DataType dataType;
};
struct Login
{
	DataHeader dataHeader;
	char username[50] = { 0 }; // 用户名
	char password[50] = { 0 }; // 密码
	Login()
	{

		dataHeader.len = sizeof(Login);
		dataHeader.dataType = DataType::LOGIN;
	}
};
struct LoginReply
{
	DataHeader dataHeader;
	char reply[100] = { 0 }; // 登录回复信息
	LoginReply()
	{
		dataHeader.len = sizeof(LoginReply);
		dataHeader.dataType = DataType::LOGIN_REPLY;
	}
};
struct Logout
{
	DataHeader dataHeader;
	unsigned int uid;	// 登出的用户id
	Logout():uid(0)
	{
		dataHeader.len = sizeof(Logout);
		dataHeader.dataType = DataType::LOGOUT;
	}
};
struct LogoutReply
{
	DataHeader dataHeader;
	char reply[100] = { 0 }; // 登出回复信息
	LogoutReply()
	{
		dataHeader.len = sizeof(LogoutReply);
		dataHeader.dataType = DataType::LOGOUT_REPLY;
	}
};
#endif