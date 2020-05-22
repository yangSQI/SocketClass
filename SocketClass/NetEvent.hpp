#ifndef __NETEVENT_HPP__
#define __NETEVENT_HPP__
#include "SocketBase.hpp"
namespace yang
{
	// SOCKET�����¼���
	class NetEvent
	{
	public:
		/**
		* @description : �������������Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		virtual void OnNetRecv(SocketInfo* _sockInfo) = 0;
		/**
		* @description : ����ͻ����뿪��Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		virtual void OnNetLeave(SocketInfo* _sockInfo) = 0;
	};

};
#endif