#include "EventSelect.h"

EventSelect::EventSelect()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

EventSelect::~EventSelect()
{
	WSACleanup();
}

bool EventSelect::Init(int port)
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
	_serverEvent = WSACreateEvent();
	if (NULL == _serverEvent)
		return false;
	WSAEventSelect(_sockServer, _serverEvent, FD_ACCEPT);
	std::thread _thread(std::mem_fn(&EventSelect::WorkerThread), this);
	_thread.detach();
	return true;
}

void EventSelect::Run()
{
	WSAWaitForMultipleEvents(1, &_serverEvent, FALSE, WSA_INFINITE, FALSE);
	if (_clientNum >= WSA_MAXIMUM_WAIT_EVENTS)
	{
		std::chrono::seconds t(1);
		// 延时
		std::this_thread::sleep_for(t);
		return;
	}
	WSANETWORKEVENTS nwe = { 0 };
	if (!WSAEnumNetworkEvents(_sockServer, _serverEvent, &nwe))
	{
		if (nwe.lNetworkEvents & FD_ACCEPT)
		{
			if (0 == nwe.iErrorCode[FD_ACCEPT_BIT])
			{
				sockaddr addrClient = { 0 };
				int addrLen = sizeof(sockaddr);
				_sockClient[_clientNum] = accept(_sockServer, &addrClient, &addrLen);
				_clientEvent[_clientNum] = WSACreateEvent();
				WSAEventSelect(_sockClient[_clientNum], _clientEvent[_clientNum], FD_READ | FD_CLOSE | FD_WRITE);
				++_clientNum;
			}
			else
			{
				int iError = nwe.iErrorCode[FD_ACCEPT_BIT];
				printf("WSAEnumNetworkEvents failed with error code: %d\n", iError);
			}
		}
	}
}

void EventSelect::WorkerThread()
{
	char buff[1024] = { 0 };
	for (;;)
	{
		if (_clientNum == 0)
		{
			std::chrono::seconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		int index = WSAWaitForMultipleEvents(_clientNum, _clientEvent, FALSE, 1000, FALSE);
		if (WSA_WAIT_TIMEOUT == index)
			continue;
		index -= WSA_WAIT_EVENT_0;
		WSANETWORKEVENTS nwe = { 0 };
		if (!WSAEnumNetworkEvents(_sockClient[index], _clientEvent[index], &nwe))
		{
			if (nwe.lNetworkEvents & FD_READ)
			{
				if (0 == nwe.iErrorCode[FD_READ_BIT])
				{
					recv(_sockClient[index], buff, 1024, 0);
					printf("客户端: %d, 数据: %s\n", _sockClient[index], buff);
				}
			}
			else if (nwe.lNetworkEvents & FD_WRITE)
			{
				if (0 == nwe.iErrorCode[FD_WRITE_BIT])
				{

				}
			}
			else if (nwe.lNetworkEvents & FD_CLOSE)
			{
				printf("客户端: %d leval", _sockClient);
			}
		}
	}
}
