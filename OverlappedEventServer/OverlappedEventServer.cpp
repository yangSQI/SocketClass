#include "OverlappedEventServer.h"

OverlappedEventServer::OverlappedEventServer()
{
	WSAData wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

OverlappedEventServer::~OverlappedEventServer()
{
	try
	{
		WSACleanup();
	}
	catch (...) {}
}

bool OverlappedEventServer::Init(int port)
{
	_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET  == _sockServer)
	{
		perror("socket error");
		return false;
	}
	_addrServer.sin_family = AF_INET;
	_addrServer.sin_port = htons(port);
	_addrServer.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(_sockServer, (sockaddr*)&_addrServer, sizeof(sockaddr));
	if (SOCKET_ERROR == ret)
	{
		perror("bind error");
		return false;
	}
	ret = listen(_sockServer, SOMAXCONN);
	if (SOCKET_ERROR == ret)
	{
		perror("listen error");
		return false;
	}
	std::thread workerThread(std::mem_fn(&OverlappedEventServer::WorkerThread), this);
	workerThread.detach();
	return true;
}

void OverlappedEventServer::Run()
{
	sockaddr_in addrClient = { 0 };
	int addrLen = sizeof(sockaddr);
	_sockClient[_clientNum] = accept(_sockServer, (sockaddr*)&addrClient, &addrLen);
	if (INVALID_SOCKET == _sockClient[_clientNum])
		return;
	printf("客户端: %d接入\n", _sockClient[_clientNum]);

	_eventClient[_clientNum] = WSACreateEvent();
	if (WSA_INVALID_EVENT == _eventClient[_clientNum])
	{
		closesocket(_sockClient[_clientNum]);
		return;
	}
	piod[_clientNum].Buffer.buf = piod[_clientNum].szMessage;
	piod[_clientNum].Buffer.len = MSGSIZE;
	piod[_clientNum].overlap.hEvent = _eventClient[_clientNum];
	WSARecv(_sockClient[_clientNum], &piod[_clientNum].Buffer, 1, &piod[_clientNum].NumberOfBytesRecvd, &piod[_clientNum].Flags, &piod[_clientNum].overlap, NULL);
	++_clientNum;
}

void OverlappedEventServer::WorkerThread()
{
	int ret, index;
	DWORD cbTransferred;
	for (;;)
	{
		ret = WSAWaitForMultipleEvents(_clientNum, _eventClient, FALSE, 1000, FALSE);
		if (WSA_WAIT_TIMEOUT == ret || WSA_WAIT_FAILED == ret)
		{
			continue;
		}
		index = ret - WSA_WAIT_EVENT_0;
		WSAResetEvent(_eventClient[index]);
		WSAGetOverlappedResult(_sockClient[index], (LPWSAOVERLAPPED)&piod[index], &cbTransferred, TRUE, &piod[index].Flags);
		if (0 == cbTransferred)
		{
			printf("客户端: %d level\n", _sockClient[index]);
			SocketQuit(index);
		}
		else
		{
			piod[index].szMessage[cbTransferred] = 0;
			printf("客户端: %d, 数据: %s\n", _sockClient[index], piod[index].szMessage);
			WSARecv(_sockClient[index], &piod[index].Buffer, 1, &piod[index].NumberOfBytesRecvd, &piod[index].Flags, &piod[index].overlap, NULL);
		}
	}
}

void OverlappedEventServer::SocketQuit(int index)
{
	closesocket(_sockClient[index]);
	WSACloseEvent(_eventClient[index]);
	if (index != _clientNum - 1)
	{
		_sockClient[index] = _sockClient[_clientNum - 1];
		_eventClient[index] = _eventClient[_clientNum - 1];
		piod[index].overlap.hEvent = piod[index].overlap.hEvent;
	}
	--_clientNum;
}
