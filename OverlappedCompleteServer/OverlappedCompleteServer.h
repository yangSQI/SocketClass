#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
struct OverGather
{
	WSAOVERLAPPED	olp = { 0 };
	WSABUF 			buffer = { 1024, szMessage };
	SOCKET 			sock = 0;
	char			szMessage[1024];
	DWORD         	NumberOfBytesRecvd = 0;  
	DWORD         	Flags = 0;  
};
class OverlappedCompleteServer
{
protected:
	SOCKET _sockServer = INVALID_SOCKET;
	sockaddr_in _addrServer = { 0 };
	OverGather _overGather[WSA_MAXIMUM_WAIT_EVENTS];
	int _clientNum = 0;
public:
	OverlappedCompleteServer();
	~OverlappedCompleteServer();
	bool Init(int port);
	void Run();
};
void CALLBACK CompletionRoutine(
    IN DWORD dwError,
    IN DWORD cbTransferred,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN DWORD dwFlags
);

