#ifndef __SOCKETHANDLE_H__
#define __SOCKETHANDLE_H__
#include "SocketBase.hpp"
#include <list>
#include <mutex>
namespace yang
{
	class SocketHandle : public SocketBase
	{
	#define TAKE_OUT_NUM FD_SETSIZE / 10						// ���м��һ���ó����ٸ��ͻ���
	private:
		typedef char* (*FnProcRecvMsg)(SocketInfo* _sockInfo);  // ����ͻ���SOCKET�Ľ�����Ϣ
	private:
		static int _sockHandleCount;							// ��ǰ��Ķ�������,��ϵ���м��Ķ�д�ϲ�����
		static std::list<SocketInfo*> _listSockInfoMedium;		// �ͻ���SOCKET�б�,��Ϊ�м��,���߳�ʹ��
		static int _listSockInfoMediumLen;						// �м��ͻ�������
		static std::mutex _mutex;								// �м�����
		std::list<SocketInfo*> _listSockInfo;					// �ͻ���SOCKET�б�
		int _listSockInfoLen = 0;								// �ͻ���SOCKET�б�����
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
				delete _sockInfo;		// �ͷſͻ���SOCKET�б�
			}
			if (_sockHandleCount <= 0) // �ͷ��м���SOCKET��Ϣ�б�
			{
				for (auto _sockInfo : _listSockInfoMedium)
				{
					delete _sockInfo;
				}
			}
		}
		/**
		* @description : ���м�����SOCKET
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		static void push_back(SocketInfo* _sockInfo)
		{
			if (_sockHandleCount > 1)
				std::lock_guard<std::mutex> lg(SocketHandle::_mutex); // lock_guard �ڹ��캯������������������������
			SocketHandle::_listSockInfoMedium.push_back(_sockInfo);
		}
		/**
		* @description : ���м��SOCKET�б�ȡ���ŵ��ͻ���SOCKET�б���
		*/
		void get_sock_info()
		{
			// �ȼ���м���Ƿ�������
			if (_listSockInfoMedium.empty()) return;
			if (_sockHandleCount > 1)
			{
				std::lock_guard<std::mutex> lg(SocketHandle::_mutex);
			}
			// �ٴμ��
			if (_listSockInfoMedium.empty()) return;
			// һ��ȡ���ĳ��� 
			int takeOutLen = _listSockInfoMedium.size() >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _listSockInfoMedium.size();
			// ���㵱ǰ���е�SOCKINFO����
			_listSockInfoLen += takeOutLen;
			// �м��ĵ�����
			auto it = _listSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_listSockInfo.push_back(*it);
				//it = _listSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : ����_sockInfo����
		* @return : ���ؿͻ����б�����
		*/
		int get_len()
		{
			return _listSockInfo.size();
			//return _listSockInfoLen;
		}
		/**
		* @description : �����м��sockinfo����
		* @return : ���ؿͻ����б�����
		*/
		int get_mediu_len()
		{
			return _listSockInfoMediumLen;
		}
		/**
		* @description : �Կͻ����б�������ݴ���
		* @param : ��������
		* @return : ����ֵ����
		*/
		void recv_data()
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			// ��ǰ�ͻ����б��β��������
			auto itEnd = _listSockInfo.end();
			// sock���ֵ����liunx��mac��ʹ��
			int sockMax = 0;
			// ��ʱָ��SocketInfo��ָ��
			SocketInfo* _sockInfo = NULL;
			for (auto itBegin = _listSockInfo.begin(); itBegin != itEnd; ++itBegin)
			{
				_sockInfo = *itBegin;
				FD_SET(_sockInfo->_sock, &fdRead);
				if (_sockInfo->_sock > sockMax)
					sockMax = _sockInfo->_sock;
			}
			timeval tv = { 0 };
			::select(sockMax + 1, &fdRead, NULL, NULL, &tv);
			for (auto itBegin = _listSockInfo.begin(); itBegin != itEnd; ++itBegin)
			{
				_sockInfo = *itBegin;
				if (SocketBase::recv_data(_sockInfo))
				{
					
				}
				else // �Ͽ�����
				{
					printf("socket level, ip: %s, port: %d\n", inet_ntoa(_sockInfo->_sockaddr.sin_addr), ntohs(_sockInfo->_sockaddr.sin_port));
					// �ر�SOCKET
					close(_sockInfo->_sock);
					// �ͷ�
					delete _sockInfo;
					// �ӿͻ����б���ɾ���Ͽ����ӵ�SOCKET
					itBegin = _listSockInfo.erase(itBegin);
					// ����Ƿ������,�����--�����
					if (itBegin != _listSockInfo.begin())
						--itBegin;
				}
			}
		}
	};
	int SocketHandle::_sockHandleCount = 0;
	std::list<SocketInfo*> SocketHandle::_listSockInfoMedium;
	int SocketHandle::_listSockInfoMediumLen = 0;
	std::mutex SocketHandle::_mutex;
};
#endif