#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <thread>
#include <functional>
class IocpServer
{
	struct SockInfo
	{
		SOCKET sock;
		sockaddr_in addr;
	};
	struct OverGather
	{
		WSAOVERLAPPED	olp = { 0 };
		WSABUF 			buffer = { 1024, szMessage };
		SOCKET 			sock = 0;
		char			szMessage[1024];
		DWORD         	NumberOfBytesRecvd = 0;  
		DWORD         	Flags = 0;  
	};
protected:
	SOCKET _sockServer = 0;
	sockaddr_in _addrServer = { 0 };
	HANDLE _hCompletion = NULL;		// 完成端口句柄
	int _cpuNum = 0;					// cpu核心数
public:
	IocpServer();
	~IocpServer();
	bool Init(int port);
	void Run();
	void WorkerThread();
};

