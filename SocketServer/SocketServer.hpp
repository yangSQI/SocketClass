#ifndef __SOCKETSERVER_HPP__
#define __SOCKETSERVER_HPP__
#include "../SocketClass/SocketBase.hpp"
namespace yang
{
	class SocketServer :
		public SocketBase
	{
	private:
		SocketBlockMode _blockMode;		// ����ģʽ
		u_int _recvSize;				// ���ջ�������С	
		u_int _sendSize;				// ���ͻ�������С
		SOCKET _sockServer;				// �����SOCKET
		sockaddr_in _addrServer;		// �����SOCKET��Ϣ�ṹ��
	public:
		/**
		* @description : ���캯��
		* @param : mode: ����ģʽ, recvSize: ����ÿ���ͻ���SOCK�Ļ������ֽڴ�С
		*/
		SocketServer(SocketBlockMode mode = SocketBlockMode::BLOCK, u_int recvSize = 10240, u_int sendSize = 10240) :_blockMode(mode), _recvSize(recvSize), _sendSize(sendSize)
		{

		}
		virtual ~SocketServer()
		{

		}
	public:
		/**
		* @description : �󶨷����SOCKET�˿ںźͼ����˿�
		* @param : port: �˿ں� connNum: listen������
		* @return :	������� 
		*/
		bool bindAndListen(unsigned short port, unsigned int connNum = 5)
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

	};
};

#endif
