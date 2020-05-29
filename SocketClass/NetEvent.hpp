#ifndef __NETEVENT_HPP__
#define __NETEVENT_HPP__
#include "SocketBase.hpp"
namespace yang
{
	// SOCKET网络事件类
	class NetEvent
	{
	protected:
		static int _sockNum; // sock数量
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
		/**
		* @description : 客户端到来消息
		* @param : _sockInfo: sock信息结构体
		*/
		virtual void OnNetAccept(SocketInfo* _sockInfo) = 0;
		/**
		* @description : 取得SOCK数量
		* @return : 返回sock数量
		*/
		int getSockNum()
		{
			return _sockNum;
		}
	};
	int NetEvent::_sockNum = 0;

};
#endif