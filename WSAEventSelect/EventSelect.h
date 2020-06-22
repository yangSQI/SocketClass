#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <thread>
#include <functional>
class EventSelect
{
protected:
	SOCKET _sockServer = 0;
	sockaddr_in _addrServer = { 0 };
	SOCKET _sockClient[WSA_MAXIMUM_WAIT_EVENTS] = { INVALID_SOCKET };
	WSAEVENT _clientEvent[WSA_MAXIMUM_WAIT_EVENTS];
	int _clientNum = 0;
	WSAEVENT _serverEvent;
public:
	EventSelect();
	~EventSelect();
	bool Init(int port);
	void Run();
	void WorkerThread();
};

