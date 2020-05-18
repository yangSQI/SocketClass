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
		bool set_block_mode(SOCKET& sock, SocketBlockMode mode);
	public:
		SocketBase();
		virtual ~SocketBase();
	};
};
#endif
