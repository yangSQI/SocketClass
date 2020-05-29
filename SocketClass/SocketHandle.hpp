#ifndef __SOCKETHANDLE_H__
#define __SOCKETHANDLE_H__
#include "SocketBase.hpp"
#include "NetEvent.hpp"
#include <list>
#include <vector>
#include <mutex>
namespace yang
{
	int num = 0;
	class SocketHandle : public SocketBase
	{
	#define TAKE_OUT_NUM FD_SETSIZE / 10						// 从中间层一次拿出多少个客户端
	private:
		typedef char* (*FnProcRecvMsg)(SocketInfo* _sockInfo);  // 处理客户端SOCKET的接收消息
	private:
		static int _sockHandleCount;							// 当前类的对象数量,关系到中间层的读写上不上锁
		static std::list<SocketInfo*> _recvSockInfoMedium;		// 客户端SOCKET列表,作为中间层,多线程使用
		static std::list<SocketInfo*> _sendSockInfoMedium;		// 客户端SOCKET列表,作为中间层,多线程使用
		static int _recvSockInfoMediumLen;						// 中间层接收消息的客户端数量
		static int _sendSockInfoMediumLen;						// 中间层发送消息的客户端数量
		static std::mutex _recvMutex;							// 接收消息的中间层的锁
		static std::mutex _sendMutex;							// 发送消息的中间层的锁
		std::list<SocketInfo*> _recvSockInfo;					// 接收消息客户端SOCKET列表
		std::list<SocketInfo*> _sendSockInfo;					// 发送消息客户端SOCKET列表
		int _recvSockInfoLen = 0;								// 接收消息客户端SOCKET列表数量
		int _sendSockInfoLen = 0;								// 发送消息客户端SOCKET列表数量
		static NetEvent* _netEvent;								// 客户端SOCKET网络事件处理类
		fd_set _oldFdRead = { 0 };								// 客户端未更新的话用这个加快性能
		fd_set _oldFdWrite = { 0 };								// 客户端未更新的话用这个加快性能
		bool _recvSockInfoChange = false;						// 接收消息客户端SOCKET列表是否更新过
		bool _sendSockInfoChange = false;						// 发送消息客户端SOCKET列表是否更新过
	public:
		SocketHandle()
		{
			++_sockHandleCount;
		}
		virtual ~SocketHandle()
		{
			--_sockHandleCount;
			for (auto _sockInfo : _recvSockInfo)
			{
				delete _sockInfo;		// 释放客户端SOCKET列表
			}
			if (_sockHandleCount <= 0) // 释放中间层的SOCKET信息列表
			{
				for (auto _sockInfo : _recvSockInfoMedium)
				{
					delete _sockInfo;
				}
			}
		}
		/**
		* @description : 往接收消息中间层添加SOCKINFO
		* @param : _sockInfo: SOCKET信息的结构体
		*/
		static void recv_push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_recvMutex); // lock_guard 在构造函数里加锁，在析构函数里解锁
			SocketHandle::_recvSockInfoMedium.push_back(_sockInfo);
			++_recvSockInfoMediumLen;
		}
		/**
		* @description : 往发送消息中间层添加SOCKINFO
		* @param : _sockInfo: SOCKET信息的结构体
		*/
		static void send_push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_sendMutex); // lock_guard 在构造函数里加锁，在析构函数里解锁
			SocketHandle::_sendSockInfoMedium.push_back(_sockInfo);
			++_sendSockInfoMediumLen;
		}
		/**
		* @description : 删除中间层SOCKINFO
		* @param : _sockInfo: SOCKET信息的结构体
		*/
		
		/**
		* @description : 从接收消息的中间层SOCKET列表取出放到客户端SOCKET列表中
		*/
		void get_recv_sock_info()
		{
			// 先检测中间层是否有数据
			if (_recvSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_recvMutex);
			// 再次检查
			if (_recvSockInfoMediumLen == 0) return;
			// 更新状态
			_recvSockInfoChange = true;
			// 一次取出的长度 
			int takeOutLen = _recvSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _recvSockInfoMediumLen;
			// 计算当前类中的SOCKINFO数量
			_recvSockInfoLen += takeOutLen;
			// 计算当前中间层客户端数量
			_recvSockInfoMediumLen -= takeOutLen;
			// 中间层的迭代器
			auto it = _recvSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_recvSockInfo.push_back(*it);
				it = _recvSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : 从发送消息的中间层SOCKET列表取出放到客户端SOCKET列表中
		*/
		void get_send_sock_info()
		{
			// 先检测中间层是否有数据
			if (_sendSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_sendMutex);
			// 再次检查
			if (_sendSockInfoMediumLen == 0) return;
			// 更新状态
			_sendSockInfoChange = true;
			// 一次取出的长度 
			int takeOutLen = _sendSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _sendSockInfoMediumLen;
			// 计算当前类中的SOCKINFO数量
			_sendSockInfoLen += takeOutLen;
			// 计算当前中间层客户端数量
			_sendSockInfoMediumLen -= takeOutLen;
			// 中间层的迭代器
			auto it = _sendSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_sendSockInfo.push_back(*it);
				it = _sendSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : 返回接收消息客户端_sockInfo数量
		* @return : 返回客户端列表数量
		*/
		int get_recv_len()
		{
			return _recvSockInfoLen;
		}
		/**
		* @description : 返回发送消息客户端_sockInfo数量
		* @return : 返回客户端列表数量
		*/
		int get_send_len()
		{
			return _sendSockInfoLen;
		}
		/**
		* @description : 返回接收消息中间层sockinfo数量
		* @return : 返回客户端列表数量
		*/
		static int get_recv_mediu_len()
		{
			return _recvSockInfoMediumLen;
		}
		/**
		* @description : 返回发送消息中间层sockinfo数量
		* @return : 返回客户端列表数量
		*/
		static int get_send_mediu_len()
		{
			return _sendSockInfoMediumLen;
		}
		/**
		* @description : 对客户端列表的收数据处理
		* @param : 参数描述
		* @return : 返回值描述
		*/
		void recv_data()
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			// 当前客户端列表的尾部迭代器
			auto itEnd = _recvSockInfo.end();
			// sock最大值，在liunx或mac中使用
			int sockMax = 0;
			// 临时指向SocketInfo的指针
			SocketInfo* _sockInfo = NULL;
			if (_recvSockInfoChange)
			{
				for (auto itBegin = _recvSockInfo.begin(); itBegin != itEnd; ++itBegin)
				{
					_sockInfo = *itBegin;
					FD_SET(_sockInfo->_sock, &fdRead);
					if (_sockInfo->_sock > sockMax)
						sockMax = _sockInfo->_sock;
				}
				_oldFdRead = fdRead;
				_recvSockInfoChange = false;
			}
			else
			{
				fdRead = _oldFdRead;
			}
			timeval tv = { 0 };
			::select(sockMax + 1, &fdRead, NULL, NULL, &tv);
			std::list<std::list<SocketInfo*>::iterator> leaveSockInfoIt; // 存储将要离开的客户端列表的迭代器
			// 循环处理客户端列表的socket
			for (auto itBegin = _recvSockInfo.begin(); itBegin != itEnd; ++itBegin)
			{
				_sockInfo = *itBegin;
				if (FD_ISSET(_sockInfo->_sock, &fdRead))
				{
					if (SocketBase::recv_data(_sockInfo))
					{
						// 接收消息
						_netEvent->OnNetRecv(_sockInfo);
					}
					else // 断开连接
					{
						leaveSockInfoIt.push_back(itBegin);
					}
				}
			}
			if (!leaveSockInfoIt.empty())
			{
				// 更新客户端状态
				_recvSockInfoChange = true;
				auto itEnd = leaveSockInfoIt.end();
				for (auto itBegin = leaveSockInfoIt.begin(); itBegin != itEnd; ++itBegin)
				{
					auto it = *itBegin;
					_sockInfo = *it;
					// 离开消息
					_netEvent->OnNetLeave(_sockInfo);
					// 关闭SOCKET
					close(_sockInfo->_sock);
					// 释放
					delete _sockInfo;
					// 从客户端列表中删除断开连接的SOCKET
					_recvSockInfo.erase(it);
					--_recvSockInfoLen;
				}
			}
		}
		/**
		* @description : 对客户端列表的发送数据处理
		*/
		void send_data()
		{
			fd_set fdWrite;
			FD_ZERO(&fdWrite);
			// 当前客户端列表的尾部迭代器
			auto itEnd = _sendSockInfo.end();
			// sock最大值，在liunx或mac中使用
			int sockMax = 0;
			// 临时指向SocketInfo的指针
			SocketInfo* _sockInfo = NULL;
			if (_sendSockInfoChange)
			{
				for (auto itBegin = _sendSockInfo.begin(); itBegin != itEnd; ++itBegin)
				{
					_sockInfo = *itBegin;
					FD_SET(_sockInfo->_sock, &fdWrite);
					if (_sockInfo->_sock > sockMax)
						sockMax = _sockInfo->_sock;
				}
				_oldFdWrite = fdWrite;
				_sendSockInfoChange = false;
			}
			else
			{
				fdWrite = _oldFdWrite;
			}
			timeval tv = { 0 };
			::select(sockMax + 1, &fdWrite, NULL, NULL, &tv);
			std::list<std::list<SocketInfo*>::iterator> leaveSockInfoIt; // 存储将要离开的客户端列表的迭代器
			// 循环处理客户端列表的socket
			for (auto itBegin = _sendSockInfo.begin(); itBegin != itEnd; ++itBegin)
			{
				_sockInfo = *itBegin;
				if (FD_ISSET(_sockInfo->_sock, &fdWrite))
				{
					SocketBase::send_data(_sockInfo);
					leaveSockInfoIt.push_back(itBegin);
				}
			}
			if (!leaveSockInfoIt.empty())
			{
				// 更新客户端状态
				_sendSockInfoChange = true;
				auto itEnd = leaveSockInfoIt.end();
				for (auto itBegin = leaveSockInfoIt.begin(); itBegin != itEnd; ++itBegin)
				{
					auto it = *itBegin;
					_sockInfo = *it;
					// 从客户端列表中删除断开连接的SOCKET
					_sendSockInfo.erase(it);
					--_sendSockInfoLen;
				}
			}
		}
		/**
		* @description : 设置网络处理类
		* @param : 参数描述
		* @return : 返回值描述
		*/
		static void SetNetEvent(NetEvent* netEvent)
		{
			_netEvent = netEvent;
		}
	};
	int SocketHandle::_sockHandleCount = 0;
	std::list<SocketInfo*> SocketHandle::_recvSockInfoMedium;
	std::list<SocketInfo*> SocketHandle::_sendSockInfoMedium;
	int SocketHandle::_recvSockInfoMediumLen = 0;
	int SocketHandle::_sendSockInfoMediumLen = 0;
	std::mutex SocketHandle::_recvMutex;
	std::mutex SocketHandle::_sendMutex;
	NetEvent* SocketHandle::_netEvent = NULL;
};
#endif