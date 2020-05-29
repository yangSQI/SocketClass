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
		static std::list<SocketInfo*> _recvSockInfoMedium;		// �ͻ���SOCKET�б�,��Ϊ�м��,���߳�ʹ��
		static std::list<SocketInfo*> _sendSockInfoMedium;		// �ͻ���SOCKET�б�,��Ϊ�м��,���߳�ʹ��
		static int _recvSockInfoMediumLen;						// �м�������Ϣ�Ŀͻ�������
		static int _sendSockInfoMediumLen;						// �м�㷢����Ϣ�Ŀͻ�������
		static std::mutex _recvMutex;							// ������Ϣ���м�����
		static std::mutex _sendMutex;							// ������Ϣ���м�����
		std::list<SocketInfo*> _recvSockInfo;					// ������Ϣ�ͻ���SOCKET�б�
		std::list<SocketInfo*> _sendSockInfo;					// ������Ϣ�ͻ���SOCKET�б�
		int _recvSockInfoLen = 0;								// ������Ϣ�ͻ���SOCKET�б�����
		int _sendSockInfoLen = 0;								// ������Ϣ�ͻ���SOCKET�б�����
		static NetEvent* _netEvent;								// �ͻ���SOCKET�����¼�������
		fd_set _oldFdRead = { 0 };								// �ͻ���δ���µĻ�������ӿ�����
		fd_set _oldFdWrite = { 0 };								// �ͻ���δ���µĻ�������ӿ�����
		bool _recvSockInfoChange = false;						// ������Ϣ�ͻ���SOCKET�б��Ƿ���¹�
		bool _sendSockInfoChange = false;						// ������Ϣ�ͻ���SOCKET�б��Ƿ���¹�
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
				delete _sockInfo;		// �ͷſͻ���SOCKET�б�
			}
			if (_sockHandleCount <= 0) // �ͷ��м���SOCKET��Ϣ�б�
			{
				for (auto _sockInfo : _recvSockInfoMedium)
				{
					delete _sockInfo;
				}
			}
		}
		/**
		* @description : ��������Ϣ�м�����SOCKINFO
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		static void recv_push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_recvMutex); // lock_guard �ڹ��캯������������������������
			SocketHandle::_recvSockInfoMedium.push_back(_sockInfo);
			++_recvSockInfoMediumLen;
		}
		/**
		* @description : ��������Ϣ�м�����SOCKINFO
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		static void send_push_back(SocketInfo* _sockInfo)
		{
			std::lock_guard<std::mutex> lg(_sendMutex); // lock_guard �ڹ��캯������������������������
			SocketHandle::_sendSockInfoMedium.push_back(_sockInfo);
			++_sendSockInfoMediumLen;
		}
		/**
		* @description : ɾ���м��SOCKINFO
		* @param : _sockInfo: SOCKET��Ϣ�Ľṹ��
		*/
		
		/**
		* @description : �ӽ�����Ϣ���м��SOCKET�б�ȡ���ŵ��ͻ���SOCKET�б���
		*/
		void get_recv_sock_info()
		{
			// �ȼ���м���Ƿ�������
			if (_recvSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_recvMutex);
			// �ٴμ��
			if (_recvSockInfoMediumLen == 0) return;
			// ����״̬
			_recvSockInfoChange = true;
			// һ��ȡ���ĳ��� 
			int takeOutLen = _recvSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _recvSockInfoMediumLen;
			// ���㵱ǰ���е�SOCKINFO����
			_recvSockInfoLen += takeOutLen;
			// ���㵱ǰ�м��ͻ�������
			_recvSockInfoMediumLen -= takeOutLen;
			// �м��ĵ�����
			auto it = _recvSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_recvSockInfo.push_back(*it);
				it = _recvSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : �ӷ�����Ϣ���м��SOCKET�б�ȡ���ŵ��ͻ���SOCKET�б���
		*/
		void get_send_sock_info()
		{
			// �ȼ���м���Ƿ�������
			if (_sendSockInfoMediumLen == 0) return;
			std::lock_guard<std::mutex> lg(_sendMutex);
			// �ٴμ��
			if (_sendSockInfoMediumLen == 0) return;
			// ����״̬
			_sendSockInfoChange = true;
			// һ��ȡ���ĳ��� 
			int takeOutLen = _sendSockInfoMediumLen >= TAKE_OUT_NUM ? TAKE_OUT_NUM : _sendSockInfoMediumLen;
			// ���㵱ǰ���е�SOCKINFO����
			_sendSockInfoLen += takeOutLen;
			// ���㵱ǰ�м��ͻ�������
			_sendSockInfoMediumLen -= takeOutLen;
			// �м��ĵ�����
			auto it = _sendSockInfoMedium.begin();
			for (int i = 0; i < takeOutLen; ++i)
			{
				_sendSockInfo.push_back(*it);
				it = _sendSockInfoMedium.erase(it);
			}
		}
		/**
		* @description : ���ؽ�����Ϣ�ͻ���_sockInfo����
		* @return : ���ؿͻ����б�����
		*/
		int get_recv_len()
		{
			return _recvSockInfoLen;
		}
		/**
		* @description : ���ط�����Ϣ�ͻ���_sockInfo����
		* @return : ���ؿͻ����б�����
		*/
		int get_send_len()
		{
			return _sendSockInfoLen;
		}
		/**
		* @description : ���ؽ�����Ϣ�м��sockinfo����
		* @return : ���ؿͻ����б�����
		*/
		static int get_recv_mediu_len()
		{
			return _recvSockInfoMediumLen;
		}
		/**
		* @description : ���ط�����Ϣ�м��sockinfo����
		* @return : ���ؿͻ����б�����
		*/
		static int get_send_mediu_len()
		{
			return _sendSockInfoMediumLen;
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
			auto itEnd = _recvSockInfo.end();
			// sock���ֵ����liunx��mac��ʹ��
			int sockMax = 0;
			// ��ʱָ��SocketInfo��ָ��
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
			std::list<std::list<SocketInfo*>::iterator> leaveSockInfoIt; // �洢��Ҫ�뿪�Ŀͻ����б�ĵ�����
			// ѭ������ͻ����б��socket
			for (auto itBegin = _recvSockInfo.begin(); itBegin != itEnd; ++itBegin)
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
						leaveSockInfoIt.push_back(itBegin);
					}
				}
			}
			if (!leaveSockInfoIt.empty())
			{
				// ���¿ͻ���״̬
				_recvSockInfoChange = true;
				auto itEnd = leaveSockInfoIt.end();
				for (auto itBegin = leaveSockInfoIt.begin(); itBegin != itEnd; ++itBegin)
				{
					auto it = *itBegin;
					_sockInfo = *it;
					// �뿪��Ϣ
					_netEvent->OnNetLeave(_sockInfo);
					// �ر�SOCKET
					close(_sockInfo->_sock);
					// �ͷ�
					delete _sockInfo;
					// �ӿͻ����б���ɾ���Ͽ����ӵ�SOCKET
					_recvSockInfo.erase(it);
					--_recvSockInfoLen;
				}
			}
		}
		/**
		* @description : �Կͻ����б�ķ������ݴ���
		*/
		void send_data()
		{
			fd_set fdWrite;
			FD_ZERO(&fdWrite);
			// ��ǰ�ͻ����б��β��������
			auto itEnd = _sendSockInfo.end();
			// sock���ֵ����liunx��mac��ʹ��
			int sockMax = 0;
			// ��ʱָ��SocketInfo��ָ��
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
			std::list<std::list<SocketInfo*>::iterator> leaveSockInfoIt; // �洢��Ҫ�뿪�Ŀͻ����б�ĵ�����
			// ѭ������ͻ����б��socket
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
				// ���¿ͻ���״̬
				_sendSockInfoChange = true;
				auto itEnd = leaveSockInfoIt.end();
				for (auto itBegin = leaveSockInfoIt.begin(); itBegin != itEnd; ++itBegin)
				{
					auto it = *itBegin;
					_sockInfo = *it;
					// �ӿͻ����б���ɾ���Ͽ����ӵ�SOCKET
					_sendSockInfo.erase(it);
					--_sendSockInfoLen;
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
	std::list<SocketInfo*> SocketHandle::_recvSockInfoMedium;
	std::list<SocketInfo*> SocketHandle::_sendSockInfoMedium;
	int SocketHandle::_recvSockInfoMediumLen = 0;
	int SocketHandle::_sendSockInfoMediumLen = 0;
	std::mutex SocketHandle::_recvMutex;
	std::mutex SocketHandle::_sendMutex;
	NetEvent* SocketHandle::_netEvent = NULL;
};
#endif