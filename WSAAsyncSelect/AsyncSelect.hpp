#pragma 
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <thread>
#include <functional>

#define WM_SOCKET (WM_USER + 100)


class AsyncSelect
{
public:
	static int _classNum;
	SOCKET sockServer;
	sockaddr_in addrServer = { 0 };
protected:

public:
	AsyncSelect(int port, HWND hWnd)
	{
		++_classNum;
		WSAData wsaData = { 0 };
		WSAStartup(MAKEWORD(2, 3), &wsaData);
		sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		addrServer.sin_family = AF_INET;
		addrServer.sin_port = htons(port);
		addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
		// °ó¶¨¶Ë¿Ú
		bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));
		// ¼àÌý¶Ë¿Ú
		listen(sockServer, SOMAXCONN);
		WSAAsyncSelect(sockServer, hWnd, WM_SOCKET, FD_ACCEPT|FD_CLOSE);
	}
	~AsyncSelect()
	{
		if (!_classNum)
			WSACleanup();
	}
	void Run()
	{

	}
};
int AsyncSelect::_classNum = 0;
