#include "OverlappedCompleteServer.h"

OverlappedCompleteServer::OverlappedCompleteServer()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

OverlappedCompleteServer::~OverlappedCompleteServer()
{
	try
	{
		WSACleanup();
	}
	catch (...) {}
}

bool OverlappedCompleteServer::Init(int port)
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
	return false;
}

void OverlappedCompleteServer::Run()
{
	sockaddr addrClient = { 0 };
	int addrLen = sizeof(sockaddr);
	OverGather* lpOverGather = new OverGather;
	lpOverGather->sock = accept(_sockServer, (sockaddr*)&addrClient, &addrLen);
	if (INVALID_SOCKET == lpOverGather->sock)
	{
		delete lpOverGather;
		return;
	}
	WSARecv(lpOverGather->sock, &lpOverGather->buffer, 1, &lpOverGather->NumberOfBytesRecvd, &lpOverGather->Flags, &lpOverGather->olp, &CompletionRoutine);
	SleepEx(1000, TRUE);
}

void CALLBACK CompletionRoutine(IN DWORD dwError, IN DWORD cbTransferred, IN LPWSAOVERLAPPED lpOverlapped, IN DWORD dwFlags)
{
	OverGather* lpOverGather = (OverGather*)lpOverlapped;
	if (dwError != 0 || cbTransferred == 0)
	{
		// 关闭客户端sock
		printf("客户端: %d Level\n", lpOverGather->sock);
		closesocket(lpOverGather->sock);
		delete lpOverGather;
	}
	else
	{
		printf("客户端: %d, 数据: %s\n", lpOverGather->sock, lpOverGather->szMessage);
		WSARecv(lpOverGather->sock, &lpOverGather->buffer, 1, &lpOverGather->NumberOfBytesRecvd, &lpOverGather->Flags, &lpOverGather->olp, &CompletionRoutine);
	}
}
