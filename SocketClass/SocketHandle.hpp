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
		static NetEvent* _netEvent;								// �ͻ���SOCKET�����¼�������
		fd_set _oldFdRead = { 0 };								// �ͻ���δ���µĻ�������ӿ�����
		bool _sockInfoChange = false;							// �ͻ���SOCKET�б��Ƿ���¹�
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
		* @description : ���м�����SOCKINFO
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		static void push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_mutex); // lock_guard �ڹ��캯������������������������
			SocketHandle::_listSockInfoMedium.push_back(_sockInfo);
			++_listSockInfoMediumLen;
		}
		/**
		* @description : ɾ���м��SOCKINFO
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		
		/**
		* @description : ���м��SOCKET�б�ȡ���ŵ��ͻ���SOCKET�б���
		*/
		void get_sock_info()
		{
			// �ȼ���м���Ƿ�������
			if (_listSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_mutex);
			// �ٴμ��
			if (_listSockInfoMediumLen == 0) return;
			// ����״̬
			_sockInfoChange = true;
			// һ��ȡ���ĳ��� 
			int takeOutLen = _listSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _listSockInfoMediumLen;
			// ���㵱ǰ���е�SOCKINFO����
			_listSockInfoLen += takeOutLen;
			// ���㵱ǰ�м��ͻ�������
			_listSockInfoMediumLen -= takeOutLen;
			// �м��ĵ�����
			auto it = _listSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_listSockInfo.push_back(*it);
				it = _listSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : ����_sockInfo����
		* @return : ���ؿͻ����б�����
		*/
		int get_len()
		{
			return _listSockInfoLen;
		}
		/**
		* @description : �����м��sockinfo����
		* @return : ���ؿͻ����б�����
		*/
		static int get_mediu_len()
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
			// ѭ������ͻ����б��socket
			for (auto itBegin = _listSockInfo.begin(); itBegin != itEnd; ++itBegin)
			{
				_sockInfo = *itBegin;
				if (FD_ISSET(_sockInfo->_sock, &fdRead))
				{
					if (SocketBase::recv_data(_sockInfo))
					{
						// ������Ϣ
						_netEvent->OnNetRecv(_sockInfo);
					}
					else // �Ͽ�����
					{
						// �뿪��Ϣ
						_netEvent->OnNetLeave(_sockInfo);
						// �ر�SOCKET
						close(_sockInfo->_sock);
						// �ͷ�
						delete _sockInfo;
						// �ӿͻ����б���ɾ���Ͽ����ӵ�SOCKET
						itBegin = _listSockInfo.erase(itBegin);
						// ����Ƿ������,�����--�����
						if (itBegin != _listSockInfo.begin())
							--itBegin;
						// ���¿ͻ���״̬
						_sockInfoChange = true;
					}
				}
			}
		}
		/**
		* @description : �������紦����
		* @param : ��������
		* @return : ����ֵ����
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