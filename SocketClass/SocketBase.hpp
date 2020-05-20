#ifndef __SOCKETBASE_HPP__
#define __SOCKETBASE_HPP__
	#ifdef _WIN32
		#define FD_SETSIZE 1000
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
#include <cstdlib>
namespace yang
{
	struct SocketInfo
	{
	public:
		SOCKET _sock = -1;				// SOCKET
		sockaddr_in _sockaddr = { 0 };  // sockaddr
		char* _sendStart = nullptr;		// 发送缓冲区起始点
		char* _sendLast  = nullptr;		// 发送缓冲区当前指向
		char* _recvStart = nullptr;		// 接收缓冲区起始点
		char* _recvLast  = nullptr;		// 接收缓冲区当前指向
		int _sendLen	 = 0;			// 发送缓冲区大小
		int _recvLen	 = 0;			// 接收缓冲区大小
		~SocketInfo()
		{
			if (_sendStart != nullptr)
				free(_sendStart);
			if (_recvStart != nullptr)
				free(_recvStart);
		}
	};
	// SOCKET阻塞模式
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
		bool set_block_mode(SOCKET sock, SocketBlockMode mode)
		{
			#ifdef _WIN32
				u_long blockMode = (u_long)mode;
				if (::ioctlsocket(sock, FIONBIO, &blockMode)) // 设置SOCKET阻塞模式
					return false;
				else
					return true;
			#else
				int blockMode = (int)mode;
				int flags = 0;
				flags = ::fcntl(sock, F_GETFL, 0);				//获取文件的flags值
				if (flags != 0) return false;
				if (blockMode)
					flags = ::fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//设置成非阻塞模式
				else
					flags = ::fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//设置成阻塞模式
				if (flags != 0) return false;
			#endif
		}
		/**
		* @description : 关闭SOCKET套接字
		* @param : 传入关闭的SOCKET 
		* @return : 返回真假
		*/
		bool close(SOCKET sock)
		{
			#ifdef _WIN32
				if (::closesocket(sock)) return false;
			#else
				if (::close(sock)) return false;
			#endif
			return true;
		}
		/**
		* @description : 接收数据
		* @param : SocketInfo*: 指向SOCKET的信息结构体
		* @return : 返回真假, 返回false代表断开socket连接
		*/
		bool recv_data(SocketInfo* _sockInfo)
		{
			// 接收缓冲区的剩余长度
			int surplusLen = _sockInfo->_recvLen - (_sockInfo->_recvLast - _sockInfo->_recvStart);
			// 检测剩余长度如果大于等于_recv缓冲区长度则返回
			if (surplusLen <= 0)
				return true;
			int ret = ::recv(_sockInfo->_sock, _sockInfo->_recvLast, surplusLen, 0);
			if (-1 == ret)
			{
				return false;
			}
			else
			{
				_sockInfo->_recvLast += ret;
				return true;
			}
		}
		/**
		* @description : 发送数据
		* @param : SocketInfo*: 指向SOCKET的信息结构体
		* @return : 失败返回-1, 成功返回发送的数据字节大小
		*/
		int send_data(SocketInfo* _sockInfo)
		{
			// 发送的数据大小
			int sendDataLen = _sockInfo->_sendLast - _sockInfo->_sendStart;
			return ::send(_sockInfo->_sock, _sockInfo->_sendStart, sendDataLen, 0);
		}
	public:
		SocketBase()
		{
			// 当sock个数为0时释放dll
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					WSADATA wsaData;
					// 加载WinSock2的dll
					if (::WSAStartup(WINSOCK_VERSION, &wsaData))
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
				if (::WSACleanup())
					throw "WSACleanup_error";
				#endif
			}
		}
	};
	u_int SocketBase::_sockBaseCount = 0;
};
#endif
