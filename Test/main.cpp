#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <tchar.h>
int main(int argc, char* argv[])
{
	WSAData wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(88);
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&sockAddr, sizeof(sockaddr)))
	{
		perror("bind error");
		return -1;
	}
	if (SOCKET_ERROR == listen(sock, SOMAXCONN))
	{
		perror("listen error");
		return -1;
	}
	sockaddr_in addrClient = { 0 };
	int addrLen = sizeof(sockaddr);
	for (;;)
	{ 
		SOCKET sockClient = accept(sock, (sockaddr*)&addrClient, &addrLen);
		if (INVALID_SOCKET == sockClient)
		{
			perror("accept error");
			return -1;
		}
		for (;;)
		{ 
			char buff1[1024] = { 0 };
			wchar_t buff2[1024] = { 0 };
			int len = 0;
			if (-1 == (len = recv(sockClient, (char*)buff1, 1024, 0)))
			{
				printf("退出\n");
				closesocket(sockClient);
				break;
			}
			MultiByteToWideChar(CP_UTF8, 0, buff1, -1, buff2, 2048);
			MessageBoxW(0, buff2, 0, 0);
			char buff3[1024] = { 0 };
			WideCharToMultiByte(CP_UTF8, 0, L"收到数据了", -1, buff3, 1024, 0, 0);
			send(sockClient, buff3, 15, 0);
		}
	}
	WSACleanup();
	
	return 0;
}