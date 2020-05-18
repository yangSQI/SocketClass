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
		#define SOCKET int
#define u_int unsigned int
	#endif
#include <cstdio>
// SOCKET阻塞模式
namespace yang
{
	enum class SocketBlockMode
	{
		BLOCK = 0,			// 阻塞模式
		NON_BLOCK_ING		// 非阻塞模式
	};
	class SocketBase
	{
	private:
		static u_int _sockBaseCount; // 记录当前程序有多少SocketBase对象,主要用于windows平台上WinSock_dll的载入和释放
	protected:
		/**
		* @description : 设置SOCKET阻塞模式
		* @param : sock: 套接字, mode: 阻塞模式
		* @return : 成功返回true, 失败返回false 
		*/
		bool set_block_mode(SOCKET& sock, SocketBlockMode mode)
		{
			#ifdef _WIN32
				u_long blockMode = (u_long)mode;
				if (ioctlsocket(sock, FIONBIO, &blockMode)) // 设置SOCKET阻塞模式
					return false;
				else
					return true;
			#else
				int blockMode = (int)mode;
				int flags = 0;
				flags = fcntl(sock, F_GETFL, 0);				//获取文件的flags值
				if (flags != 0) return false;
				if (blockMode)
					flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//设置成非阻塞模式
				else
					flags = fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//设置成阻塞模式
				if (flags != 0) return false;
			#endif
		}
	public:
		SocketBase()
		{
			printf("SocketBase构造哈哈\n");
			// 当sock个数为0时释放dll
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					WSADATA wsaData;
					// 加载WinSock2的dll
					if (WSAStartup(WINSOCK_VERSION, &wsaData))
						throw "WSAStartup_error";
				#endif
			}
			++_sockBaseCount;
		}
		virtual ~SocketBase()
		{
			--_sockBaseCount;
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					// 释放DLL
				if (WSACleanup())
					throw "WSACleanup_error";
				#endif
			}
		}
	};
};
u_int yang::SocketBase::_sockBaseCount = 0;
#endif
