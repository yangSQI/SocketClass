#ifndef __NETEVENT_HPP__
#define __NETEVENT_HPP__
#include "SocketBase.hpp"
namespace yang
{
	// SOCKET网络事件类
	class NetEvent
	{
	public:
		/**
		* @description : 处理网络接收消息
		* @param : _sockInfo: sock信息结构体
		*/
		virtual void OnNetRecv(SocketInfo* _sockInfo) = 0;
		/**
		* @description : 处理客户端离开消息
		* @param : _sockInfo: sock信息结构体
		*/
		virtual void OnNetLeave(SocketInfo* _sockInfo) = 0;
	};

};
#endif