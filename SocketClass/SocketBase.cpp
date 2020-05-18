#include "SocketBase.h"
using namespace yang;

u_int SocketBase::_sockBaseCount = 0;

SocketBase::SocketBase()
{
	// 当sock个数为0时释放dll
    if (_sockBaseCount == 0)
    {
        #ifdef _WIN32
            WSADATA wsaData;
			// 加载WinSock2的dll
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
			// 释放DLL
		if (WSACleanup())
			throw "WSACleanup_error";
		#endif
	}
	
}

bool SocketBase::set_block_mode(SOCKET& sock, SocketBlockMode mode)
{
#ifdef _WIN32
	u_long blockMode = (u_long)mode;
	if (ioctlsocket(sock, FIONBIO, &blockMode)) // 设置SOCKET阻塞模式
		return false;
	else
		return true;
#else
	int blockMode = (int)mode;
	int flags = 0;
	flags = fcntl(sock, F_GETFL, 0);				//获取文件的flags值
	if (flags != 0) return false;
	if (blockMode)
		flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//设置成非阻塞模式
	else
		flags = fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//设置成阻塞模式
	if (flags != 0) return false;
#endif
}
