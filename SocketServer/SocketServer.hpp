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
		SocketBlockMode _blockMode;							// ����ģʽ
		u_int _recvSize;									// ���ջ�������С	
		u_int _sendSize;									// ���ͻ�������С
		SOCKET _sockServer;									// �����SOCKET
		sockaddr_in _addrServer = { 0 };					// �����SOCKET��Ϣ�ṹ��
		int _recvThreadNum = 0;								// ������Ϣ�߳�����
		int _sendThreadNum = 0;								// ������Ϣ�߳�����
		std::mutex _mutex;									// _listSockInfo��������
		std::vector<SocketHandle*> _recvSockHandleGather;	// SocketHandle�������б�
		std::vector<SocketHandle*> _sendSockHandleGather;	// SocketHandle�������б�
		NetEvent* _netEvent = new NetEventServer;			// ������Ϣ������
	public:
		/**
		* @description : ���캯��
		* @param : mode: ����ģʽ, recvSize: ����ÿ���ͻ���SOCK�Ļ������ֽڴ�С
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
		* @description : �󶨷����SOCKET�˿ںźͼ����˿�
		* @param : port: �˿ں� connNum: listen������
		* @return :	������� 
		*/
		bool bind_and_listen(unsigned short port, unsigned int connNum = 5)
		{
			_addrServer.sin_family = AF_INET;					// ��ַ��淶
			_addrServer.sin_port = htons(port);				// �ó��򱾵ض˿ں�
			_addrServer.sin_addr.s_addr = htonl(INADDR_ANY);	// �󶨱���ip��ַ

			if (::bind(_sockServer, (sockaddr*)&_addrServer, sizeof(sockaddr)) == -1 || ::listen(_sockServer, connNum) == -1)
			{
				return false;
			}
			return true;
		}
		/**
		* @description : �����̴߳���SOCKET���ܺͷ�������
		* @param : threadNum: �����̵߳�����
		* @return : ����ֵ����
		*/
		void start(int recvThreadNum = 4, int sendThreadNum = 1)
		{
			_recvThreadNum = recvThreadNum;
			_sendThreadNum = sendThreadNum;
			for (int i = 0; i < _recvThreadNum; ++i)
			{
				// ����̴߳����SOCKET������
				_recvSockHandleGather.push_back(new SocketHandle);
				std::thread _thread(std::mem_fn(&SocketServer::onRunRecvData), this, i);
				_thread.detach();
			}
			for (int i = 0; i < _sendThreadNum; ++i)
			{
				// ����̴߳����SOCKET������
				_sendSockHandleGather.push_back(new SocketHandle);
				std::thread _thread(std::mem_fn(&SocketServer::onRunSendData), this, i);
				_thread.detach();
			}
		}
		/**
		* @description : ����SOCKET�ͻ��˵����Ĵ���, ������ӽ�SOCKET�ͻ����б���
		* @param : ��������
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
		* @description : ����ͻ���SOCKET�Ľ���
		* @param : ��������
		* @return : ����ֵ����
		*/
		void onRunRecvData(int threadIndex)
		{
			SocketHandle* _sockHandle = _recvSockHandleGather.at(threadIndex);
			for (;;)
			{
				//printf("�߳�: %d, �ͻ�������: %d, �м������: %d\n", threadIndex, _sockHandle->get_len(), _sockHandle->get_mediu_len());
				if (_sockHandle->get_recv_len() == 0 && _sockHandle->get_recv_mediu_len() == 0)
				{
					std::chrono::milliseconds t(1);
					// ��ʱ
					std::this_thread::sleep_for(t);
					continue;
				}
				if (_sockHandle->get_recv_mediu_len() > 0)
				{
					bool flag = true;
					// ��ǰSOCKET�������еĿͻ����б���������̵߳Ļ�,�Ͳ����м��ȡ����
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
						// �ȴ��м��ȡ��SOCKET
						_sockHandle->get_recv_sock_info();
					}
				}
				_sockHandle->recv_data();
				//std::chrono::seconds t(1);
				//// ��ʱ
				//std::this_thread::sleep_for(t);
				//printf("�߳�: %d, _sockInfo����: %d\n", threadIndex, _sockHandle->_listSockInfo.size());
			}
		}
		/**
		* @description : ����ͻ���SOCKET�ķ���
		* @param : ��������
		* @return : ����ֵ����
		*/
		void onRunSendData(int threadIndex)
		{
			SocketHandle* _sockHandle = _sendSockHandleGather.at(threadIndex);
			for (;;)
			{
				//printf("�߳�: %d, �ͻ�������: %d, �м������: %d\n", threadIndex, _sockHandle->get_len(), _sockHandle->get_mediu_len());
				if (_sockHandle->get_send_len() == 0 && _sockHandle->get_send_mediu_len() == 0)
				{
					std::chrono::milliseconds t(1);
					// ��ʱ
					std::this_thread::sleep_for(t);
					continue;
				}
				if (_sockHandle->get_send_mediu_len() > 0)
				{
					bool flag = true;
					// ��ǰSOCKET�������еĿͻ����б���������̵߳Ļ�,�Ͳ����м��ȡ����
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
						// �ȴ��м��ȡ��SOCKET
						_sockHandle->get_send_sock_info();
					}
				}
				_sockHandle->send_data();
				//std::chrono::seconds t(1);
				//// ��ʱ
				//std::this_thread::sleep_for(t);
				//printf("�߳�: %d, _sockInfo����: %d\n", threadIndex, _sockHandle->_listSockInfo.size());
			}
		}

		/**
		* @description : ���ÿͻ����б�����紦����
		* @param : netEvent: ���紦����
		*/
		void SetNetEvent(NetEvent* netEvent)
		{
			SocketHandle::SetNetEvent(netEvent);
		}
	protected:
		/**
		* @description : �����SOCKET���տͻ���SOCKET����
		* @param : _sockInfo: ����SOCKET��Ϣ�ṹ��ָ��
		* @return : �������
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
