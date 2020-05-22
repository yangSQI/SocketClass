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
		static std::list<SocketInfo*> _listSockInfoMedium;		// 客户端SOCKET列表,作为中间层,多线程使用
		static int _listSockInfoMediumLen;						// 中间层客户端数量
		static std::mutex _mutex;								// 中间层的锁
		std::list<SocketInfo*> _listSockInfo;					// 客户端SOCKET列表
		int _listSockInfoLen = 0;								// 客户端SOCKET列表数量
		static NetEvent* _netEvent;								// 客户端SOCKET网络事件处理类
		fd_set _oldFdRead = { 0 };								// 客户端未更新的话用这个加快性能
		bool _sockInfoChange = false;							// 客户端SOCKET列表是否更新过
	public:
		SocketHandle()
		{
			++_sockHandleCount;
		}
		virtual ~SocketHandle()
		{
			--_sockHandleCount;
			for (auto _sockInfo : _listSockInfo)
			{
				delete _sockInfo;		// 释放客户端SOCKET列表
			}
			if (_sockHandleCount <= 0) // 释放中间层的SOCKET信息列表
			{
				for (auto _sockInfo : _listSockInfoMedium)
				{
					delete _sockInfo;
				}
			}
		}
		/**
		* @description : 往中间层添加SOCKINFO
		* @param : _sockInfo: SOCKET信息的结构体
		*/
		static void push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_mutex); // lock_guard 在构造函数里加锁，在析构函数里解锁
			SocketHandle::_listSockInfoMedium.push_back(_sockInfo);
			++_listSockInfoMediumLen;
		}
		/**
		* @description : 删除中间层SOCKINFO
		* @param : _sockInfo: SOCKET信息的结构体
		*/
		
		/**
		* @description : 从中间层SOCKET列表取出放到客户端SOCKET列表中
		*/
		void get_sock_info()
		{
			// 先检测中间层是否有数据
			if (_listSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_mutex);
			// 再次检查
			if (_listSockInfoMediumLen == 0) return;
			// 更新状态
			_sockInfoChange = true;
			// 一次取出的长度 
			int takeOutLen = _listSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _listSockInfoMediumLen;
			// 计算当前类中的SOCKINFO数量
			_listSockInfoLen += takeOutLen;
			// 计算当前中间层客户端数量
			_listSockInfoMediumLen -= takeOutLen;
			// 中间层的迭代器
			auto it = _listSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_listSockInfo.push_back(*it);
				it = _listSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : 返回_sockInfo数量
		* @return : 返回客户端列表数量
		*/
		int get_len()
		{
			return _listSockInfoLen;
		}
		/**
		* @description : 返回中间层sockinfo数量
		* @return : 返回客户端列表数量
		*/
		static int get_mediu_len()
		{
			return _listSockInfoMediumLen;
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
			auto itEnd = _listSockInfo.end();
			// sock最大值，在liunx或mac中使用
			int sockMax = 0;
			// 临时指向SocketInfo的指针
			SocketInfo* _sockInfo = NULL;
			if (_sockInfoChange)
			{
				for (auto itBegin = _listSockInfo.begin(); itBegin != itEnd; ++itBegin)
				{
					_sockInfo = *itBegin;
					FD_SET(_sockInfo->_sock, &fdRead);
					if (_sockInfo->_sock > sockMax)
						sockMax = _sockInfo->_sock;
				}
				_oldFdRead = fdRead;
				_sockInfoChange = false;
			}
			else
			{
				fdRead = _oldFdRead;
			}
			timeval tv = { 0 };
			::select(sockMax + 1, &fdRead, NULL, NULL, &tv);
			// 循环处理客户端列表的socket
			for (auto itBegin = _listSockInfo.begin(); itBegin != itEnd; ++itBegin)
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
						// 离开消息
						_netEvent->OnNetLeave(_sockInfo);
						// 关闭SOCKET
						close(_sockInfo->_sock);
						// 释放
						delete _sockInfo;
						// 从客户端列表中删除断开连接的SOCKET
						itBegin = _listSockInfo.erase(itBegin);
						// 检测是否在起点,在起点--会出错
						if (itBegin != _listSockInfo.begin())
							--itBegin;
						// 更新客户端状态
						_sockInfoChange = true;
					}
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
	std::list<SocketInfo*> SocketHandle::_listSockInfoMedium;
	int SocketHandle::_listSockInfoMediumLen = 0;
	std::mutex SocketHandle::_mutex;
	NetEvent* SocketHandle::_netEvent = NULL;
};
#endif