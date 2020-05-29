#ifndef __NETEVENT_HPP__
#define __NETEVENT_HPP__
#include "SocketBase.hpp"
namespace yang
{
	// SOCKET�����¼���
	class NetEvent
	{
	protected:
		static int _sockNum; // sock����
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
		/**
		* @description : �ͻ��˵�����Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		virtual void OnNetAccept(SocketInfo* _sockInfo) = 0;
		/**
		* @description : ȡ��SOCK����
		* @return : ����sock����
		*/
		int getSockNum()
		{
			return _sockNum;
		}
	};
	int NetEvent::_sockNum = 0;

};
#endif