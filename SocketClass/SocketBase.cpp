#include "SocketBase.h"
using namespace yang;

u_int SocketBase::_sockBaseCount = 0;

SocketBase::SocketBase()
{
	// ��sock����Ϊ0ʱ�ͷ�dll
    if (_sockBaseCount == 0)
    {
        #ifdef _WIN32
            WSADATA wsaData;
			// ����WinSock2��dll
			if (WSAStartup(WINSOCK_VERSION, &wsaData))
				throw "WSAStartup_error";
        #endif
    }
	++_sockBaseCount;
}

SocketBase::~SocketBase()
{
	--_sockBaseCount;
	if (_sockBaseCount == 0)
	{
		#ifdef _WIN32
			// �ͷ�DLL
		if (WSACleanup())
			throw "WSACleanup_error";
		#endif
	}
	
}

bool SocketBase::set_block_mode(SOCKET& sock, SocketBlockMode mode)
{
#ifdef _WIN32
	u_long blockMode = (u_long)mode;
	if (ioctlsocket(sock, FIONBIO, &blockMode)) // ����SOCKET����ģʽ
		return false;
	else
		return true;
#else
	int blockMode = (int)mode;
	int flags = 0;
	flags = fcntl(sock, F_GETFL, 0);				//��ȡ�ļ���flagsֵ
	if (flags != 0) return false;
	if (blockMode)
		flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//���óɷ�����ģʽ
	else
		flags = fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//���ó�����ģʽ
	if (flags != 0) return false;
#endif
}
