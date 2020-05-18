#ifndef __SOCKETBASE_H__
#define __SOCKETBASE_H__
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <WinSock2.h>
		#include <windows.h>
		#pragma comment(lib, "ws2_32.lib")
	#else
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <unistd.h>
		#include <fcntl.h>
	#endif
class SocketBase
{
	
};
#endif
