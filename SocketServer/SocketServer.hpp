#ifndef __SOCKETSERVER_HPP__
#define __SOCKETSERVER_HPP__
#include "../SocketClass/SocketBase.hpp"
#include "../SocketClass/SocketHandle.hpp"
#include "NetEventServer.hpp"
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
namespace yang
{
	class SocketServer :
		public SocketBase
	{
	protected:
		SocketBlockMode _blockMode;							// 阻塞模式
		u_int _recvSize;									// 接收缓冲区大小	
		u_int _sendSize;									// 发送缓冲区大小
		SOCKET _sockServer;									// 服务端SOCKET
		sockaddr_in _addrServer = { 0 };					// 服务端SOCKET信息结构体
		int _recvThreadNum = 0;								// 接收消息线程数量
		int _sendThreadNum = 0;								// 发送消息线程数量
		std::mutex _mutex;									// _listSockInfo操作的锁
		std::vector<SocketHandle*> _recvSockHandleGather;	// SocketHandle处理类列表
		std::vector<SocketHandle*> _sendSockHandleGather;	// SocketHandle处理类列表
		NetEvent* _netEvent = new NetEventServer;			// 网络消息处理类
	public:
		/**
		* @description : 构造函数
		* @param : mode: 阻塞模式, recvSize: 传入每个客户端SOCK的缓冲区字节大小
		*/
		SocketServer(SocketBlockMode mode = SocketBlockMode::BLOCK, u_int recvSize = 10240, u_int sendSize = 10240) :_blockMode(mode), _recvSize(recvSize), _sendSize(sendSize)
		{
			_sockServer = ::socket(AF_INET, SOCK_STREAM, 0); 	
			if (_sockServer == -1)
				throw "call socket error";
			if (!set_block_mode(_sockServer, _blockMode))
				throw "call set_block_mode error, param: _sockServer";
		}
		virtual ~SocketServer()
		{
			delete _netEvent;
			close(_sockServer);
			for (auto _sockHandle : _recvSockHandleGather)
			{
				delete _sockHandle;
			}
		}
	public:
		/**
		* @description : 绑定服务端SOCKET端口号和监听端口
		* @param : port: 端口号 connNum: listen处理数
		* @return :	返回真假 
		*/
		bool bind_and_listen(unsigned short port, unsigned int connNum = 5)
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
		/**
		* @description : 启动线程处理SOCKET接受和发送数据
		* @param : threadNum: 启动线程的数量
		* @return : 返回值描述
		*/
		void start(int recvThreadNum = 4, int sendThreadNum = 1)
		{
			_recvThreadNum = recvThreadNum;
			_sendThreadNum = sendThreadNum;
			for (int i = 0; i < _recvThreadNum; ++i)
			{
				// 添加线程处理的SOCKET处理类
				_recvSockHandleGather.push_back(new SocketHandle);
				std::thread _thread(std::mem_fn(&SocketServer::onRunRecvData), this, i);
				_thread.detach();
			}
			for (int i = 0; i < _sendThreadNum; ++i)
			{
				// 添加线程处理的SOCKET处理类
				_sendSockHandleGather.push_back(new SocketHandle);
				std::thread _thread(std::mem_fn(&SocketServer::onRunSendData), this, i);
				_thread.detach();
			}
		}
		/**
		* @description : 处理SOCKET客户端到来的处理, 将其添加进SOCKET客户端列表中
		* @param : 参数描述
		*/
		void onRunAccept()
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(_sockServer, &fdRead);
			timeval tv = { 0 };
			tv.tv_usec = 10;
			::select(_sockServer + 1, &fdRead, NULL, NULL, &tv);
			if (FD_ISSET(_sockServer, &fdRead))
			{
				SocketInfo* _sockInfo = new SocketInfo;
				if (!accept(_sockInfo))
				{
					delete _sockInfo;
					throw "accept error";
					return;
				}
				else
				{
					_netEvent->OnNetAccept(_sockInfo);
					if (!set_block_mode(_sockInfo->_sock, _blockMode))
					{
						throw "call set_block_mode error, param: SocketClient";
						return;
					}
					_sockInfo->_recvStart = (char*)malloc(_recvSize);
					_sockInfo->_recvLast = _sockInfo->_recvStart;
					_sockInfo->_recvLen = _recvSize;
					_sockInfo->_sendStart = (char*)malloc(_sendSize);
					_sockInfo->_sendLast = _sockInfo->_sendStart;
					_sockInfo->_sendLen = _sendSize;
					SocketHandle::recv_push_back(_sockInfo);
				}
			}
		}
		/**
		* @description : 处理客户端SOCKET的接收
		* @param : 参数描述
		* @return : 返回值描述
		*/
		void onRunRecvData(int threadIndex)
		{
			SocketHandle* _sockHandle = _recvSockHandleGather.at(threadIndex);
			for (;;)
			{
				//printf("线程: %d, 客户端数量: %d, 中间层数量: %d\n", threadIndex, _sockHandle->get_len(), _sockHandle->get_mediu_len());
				if (_sockHandle->get_recv_len() == 0 && _sockHandle->get_recv_mediu_len() == 0)
				{
					std::chrono::milliseconds t(1);
					// 延时
					std::this_thread::sleep_for(t);
					continue;
				}
				if (_sockHandle->get_recv_mediu_len() > 0)
				{
					bool flag = true;
					// 当前SOCKET处理类中的客户端列表大于其他线程的话,就不从中间层取数据
					for (int i = 0; i < _recvThreadNum; ++i)
					{
						if (i != threadIndex)
						{
							if (_sockHandle->get_recv_len() > _recvSockHandleGather.at(i)->get_recv_len())
								flag = false;
						}
					}
					if (flag)
					{
						// 先从中间层取出SOCKET
						_sockHandle->get_recv_sock_info();
					}
				}
				_sockHandle->recv_data();
				//std::chrono::seconds t(1);
				//// 延时
				//std::this_thread::sleep_for(t);
				//printf("线程: %d, _sockInfo数量: %d\n", threadIndex, _sockHandle->_listSockInfo.size());
			}
		}
		/**
		* @description : 处理客户端SOCKET的发送
		* @param : 参数描述
		* @return : 返回值描述
		*/
		void onRunSendData(int threadIndex)
		{
			SocketHandle* _sockHandle = _sendSockHandleGather.at(threadIndex);
			for (;;)
			{
				//printf("线程: %d, 客户端数量: %d, 中间层数量: %d\n", threadIndex, _sockHandle->get_len(), _sockHandle->get_mediu_len());
				if (_sockHandle->get_send_len() == 0 && _sockHandle->get_send_mediu_len() == 0)
				{
					std::chrono::milliseconds t(1);
					// 延时
					std::this_thread::sleep_for(t);
					continue;
				}
				if (_sockHandle->get_send_mediu_len() > 0)
				{
					bool flag = true;
					// 当前SOCKET处理类中的客户端列表大于其他线程的话,就不从中间层取数据
					for (int i = 0; i < _sendThreadNum; ++i)
					{
						if (i != threadIndex)
						{
							if (_sockHandle->get_recv_len() > _recvSockHandleGather.at(i)->get_recv_len())
								flag = false;
						}
					}
					if (flag)
					{
						// 先从中间层取出SOCKET
						_sockHandle->get_send_sock_info();
					}
				}
				_sockHandle->send_data();
				//std::chrono::seconds t(1);
				//// 延时
				//std::this_thread::sleep_for(t);
				//printf("线程: %d, _sockInfo数量: %d\n", threadIndex, _sockHandle->_listSockInfo.size());
			}
		}

		/**
		* @description : 设置客户端列表的网络处理类
		* @param : netEvent: 网络处理类
		*/
		void SetNetEvent(NetEvent* netEvent)
		{
			SocketHandle::SetNetEvent(netEvent);
		}
	protected:
		/**
		* @description : 服务端SOCKET接收客户端SOCKET连接
		* @param : _sockInfo: 传入SOCKET信息结构体指针
		* @return : 返回真假
		*/
		bool accept(SocketInfo* _sockInfo)
		{
			if (_sockServer == -1) return false;
			#ifdef _WIN32
				int addrlen = sizeof(sockaddr);
				_sockInfo->_sock = ::accept(_sockServer, (sockaddr*)&_sockInfo->_sockaddr, &addrlen);
			#else
				socklen_t addrlen = sizeof(sockaddr);
				_sockInfo->_sock = ::accept(_sockServer, (sockaddr*)&_sockInfo->_sockaddr, &addrlen);
			#endif
			if (_sockInfo->_sock == -1)
				return false;
			else
				return true;
		}
	};
};

#endif
