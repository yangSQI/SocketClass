#ifndef __DATAGATHER_H__
#define __DATAGATHER_H__
enum class DataType
{
	LOGIN,
	LOGOUT
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
#endif