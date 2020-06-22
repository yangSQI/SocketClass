#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
int main(int argc, char* argv[])
{

	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addrServer = { 0 };
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(88);
	addrServer.sin_addr.s_addr = INADDR_ANY;
	bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));
	listen(sockServer, SOMAXCONN);
	sockaddr addrClient;
	int addrLen = sizeof(sockaddr);
	accept(sockServer, &addrClient, &addrLen);
	return 0;
}