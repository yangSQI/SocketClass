#ifndef __SOCKETSERVER_HPP__
#define __SOCKETSERVER_HPP__
#include "../SocketClass/SocketBase.hpp"
namespace yang
{
	class SocketServer :
		public SocketBase
	{
	private:
		SocketBlockMode _blockMode;		// 阻塞模式
		u_int _recvSize;				// 接收缓冲区大小	
		u_int _sendSize;				// 发送缓冲区大小
		SOCKET _sockServer;				// 服务端SOCKET
		sockaddr_in _addrServer;		// 服务端SOCKET信息结构体
	public:
		/**
		* @description : 构造函数
		* @param : mode: 阻塞模式, recvSize: 传入每个客户端SOCK的缓冲区字节大小
		*/
		SocketServer(SocketBlockMode mode = SocketBlockMode::BLOCK, u_int recvSize = 10240, u_int sendSize = 10240) :_blockMode(mode), _recvSize(recvSize), _sendSize(sendSize)
		{

		}
		virtual ~SocketServer()
		{

		}
	public:
		/**
		* @description : 绑定服务端SOCKET端口号和监听端口
		* @param : port: 端口号 connNum: listen处理数
		* @return :	返回真假 
		*/
		bool bindAndListen(unsigned short port, unsigned int connNum = 5)
		{
			_addrServer.sin_family = AF_INET;					// 地址族规范
			_addrServer.sin_port = htons(port);				// 该程序本地端口号
			_addrServer.sin_addr.s_addr = htonl(INADDR_ANY);	// 绑定本地ip地址
			if (::bind(_sockServer, (sockaddr*)&_addrServer, sizeof(sockaddr)) == -1 || ::listen(_sockServer, connNum) == -1)
			{
				return false;
			}
			return true;
		}

	};
};

#endif
