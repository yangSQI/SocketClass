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
	char username[50] = { 0 }; // �û���
	char password[50] = { 0 }; // ����
	Login()
	{

		dataHeader.len = sizeof(Login);
		dataHeader.dataType = DataType::LOGIN;
	}
};
struct LoginReply
{
	DataHeader dataHeader;
	char reply[100] = { 0 }; // ��¼�ظ���Ϣ
	LoginReply()
	{
		dataHeader.len = sizeof(LoginReply);
		dataHeader.dataType = DataType::LOGIN_REPLY;
	}
};
struct Logout
{
	DataHeader dataHeader;
	unsigned int uid;	// �ǳ����û�id
	Logout():uid(0)
	{
		dataHeader.len = sizeof(Logout);
		dataHeader.dataType = DataType::LOGOUT;
	}
};
struct LogoutReply
{
	DataHeader dataHeader;
	char reply[100] = { 0 }; // �ǳ��ظ���Ϣ
	LogoutReply()
	{
		dataHeader.len = sizeof(LogoutReply);
		dataHeader.dataType = DataType::LOGOUT_REPLY;
	}
};
#endif