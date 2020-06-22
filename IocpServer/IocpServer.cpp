#include "IocpServer.h"

IocpServer::IocpServer()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SYSTEM_INFO si = { 0 };
	GetSystemInfo(&si);
	// si.dwNumberOfProcessors: 核心数
	_cpuNum = si.dwNumberOfProcessors;
}

IocpServer::~IocpServer()
{
	WSACleanup();
}

bool IocpServer::Init(int port)
{
	_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sockServer)
		return false;
	_addrServer.sin_family = AF_INET;
	_addrServer.sin_port = htons(port);
	_addrServer.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(_sockServer, (sockaddr*)&_addrServer, sizeof(sockaddr));
	if (SOCKET_ERROR == ret)
		return false;
	ret = listen(_sockServer, SOMAXCONN);
	if (SOCKET_ERROR == ret)
		return false;
	_hCompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _cpuNum);
	if (NULL == _hCompletion)
		return false;
	// 创建工作者线程
	for (int i = 0; i < _cpuNum; ++i)
	{
		std::thread _thread(std::mem_fn(&IocpServer::WorkerThread), this);
		_thread.detach();
	}
	return true;
}

void IocpServer::Run()
{
	SockInfo* lpSockInfo = new SockInfo;
	OverGather* lpOverGather = new OverGather;
	int addrLen = sizeof(sockaddr);
	lpSockInfo->sock = accept(_sockServer, (sockaddr*)&lpSockInfo->addr, &addrLen);
	if (INVALID_SOCKET == lpSockInfo->sock)
		return;
	CreateIoCompletionPort((HANDLE)lpSockInfo->sock, _hCompletion, (ULONG_PTR)lpSockInfo, _cpuNum);
	WSARecv(lpSockInfo->sock, &lpOverGather->buffer, 1, &lpOverGather->NumberOfBytesRecvd, &lpOverGather->Flags, &lpOverGather->olp, NULL);
}

void IocpServer::WorkerThread()
{
	SockInfo* lpSockInfo = nullptr;
	OverGather* lpOverGather = nullptr;
	DWORD trans = 0;
	for (;;)
	{
		int ret = GetQueuedCompletionStatus(_hCompletion, &trans, (PULONG_PTR)&lpSockInfo, (LPOVERLAPPED*)&lpOverGather, INFINITE);
		if (trans == 0)
		{
			printf("客户端: %d Leval\n", lpSockInfo->sock);
			closesocket(lpSockInfo->sock);
			delete lpSockInfo;
			delete lpOverGather;
		}
		else
		{
			printf("客户端: %d, 数据: %s\n", lpSockInfo->sock, lpOverGather->szMessage);
			WSARecv(lpSockInfo->sock, &lpOverGather->buffer, 1, &lpOverGather->NumberOfBytesRecvd, &lpOverGather->Flags, &lpOverGather->olp, NULL);
		}
	}
}
