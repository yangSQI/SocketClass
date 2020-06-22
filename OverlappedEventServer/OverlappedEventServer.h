#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <thread>
#include <functional>
#define MSGSIZE 1024
class OverlappedEventServer
{
typedef struct   
{  
    WSAOVERLAPPED overlap;  
    WSABUF        Buffer;  
    char          szMessage[MSGSIZE];  
    DWORD         NumberOfBytesRecvd;  
    DWORD         Flags = 0;  
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;  
protected:
	SOCKET _sockServer = 0;
	sockaddr_in _addrServer = { 0 };
	SOCKET _sockClient[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
	int _clientNum = 0;
	WSAEVENT _eventClient[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
	PER_IO_OPERATION_DATA piod[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
public:
	OverlappedEventServer();
	~OverlappedEventServer();
	bool Init(int port);			// 初始化
	void Run();						// 运行接收客户端SOCKET连接
	void WorkerThread();			// 工作者线程
	void SocketQuit(int index);		// SOCKET退出
};


