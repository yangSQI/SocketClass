#ifndef __SOCKETBASE_HPP__
#define __SOCKETBASE_HPP__
	#ifdef _WIN32
		#define FD_SETSIZE 1000
		#define WIN32_LEAN_AND_MEAN
		#include <WinSock2.h>
		#include <windows.h>
		#pragma comment(lib, "ws2_32.lib")
	#else
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <unistd.h>
		#include <fcntl.h>
		#define SOCKET int
		#define u_int unsigned int
	#endif
#include <cstdlib>
namespace yang
{
	struct SocketInfo
	{
	public:
		SOCKET _sock = -1;				// SOCKET
		sockaddr_in _sockaddr = { 0 };  // sockaddr
		char* _sendStart = nullptr;		// ���ͻ�������ʼ��
		char* _sendLast  = nullptr;		// ���ͻ�������ǰָ��
		char* _recvStart = nullptr;		// ���ջ�������ʼ��
		char* _recvLast  = nullptr;		// ���ջ�������ǰָ��
		int _sendLen	 = 0;			// ���ͻ�������С
		int _recvLen	 = 0;			// ���ջ�������С
		~SocketInfo()
		{
			if (_sendStart != nullptr)
				free(_sendStart);
			if (_recvStart != nullptr)
				free(_recvStart);
		}
	};
	// SOCKET����ģʽ
	enum class SocketBlockMode
	{
		BLOCK = 0,			// ����ģʽ
		NON_BLOCK_ING		// ������ģʽ
	};
	class SocketBase
	{
	private:
		static u_int _sockBaseCount; // ��¼��ǰ�����ж���SocketBase����,��Ҫ����windowsƽ̨��WinSock_dll��������ͷ�
		
	protected:
		/**
		* @description : ����SOCKET����ģʽ
		* @param : sock: �׽���, mode: ����ģʽ
		* @return : �ɹ�����true, ʧ�ܷ���false 
		*/
		bool set_block_mode(SOCKET sock, SocketBlockMode mode)
		{
			#ifdef _WIN32
				u_long blockMode = (u_long)mode;
				if (::ioctlsocket(sock, FIONBIO, &blockMode)) // ����SOCKET����ģʽ
					return false;
				else
					return true;
			#else
				int blockMode = (int)mode;
				int flags = 0;
				flags = ::fcntl(sock, F_GETFL, 0);				//��ȡ�ļ���flagsֵ
				if (flags != 0) return false;
				if (blockMode)
					flags = ::fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//���óɷ�����ģʽ
				else
					flags = ::fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//���ó�����ģʽ
				if (flags != 0) return false;
			#endif
		}
		/**
		* @description : �ر�SOCKET�׽���
		* @param : ����رյ�SOCKET 
		* @return : �������
		*/
		bool close(SOCKET sock)
		{
			#ifdef _WIN32
				if (::closesocket(sock)) return false;
			#else
				if (::close(sock)) return false;
			#endif
			return true;
		}
		/**
		* @description : ��������
		* @param : SocketInfo*: ָ��SOCKET����Ϣ�ṹ��
		* @return : �������, ����false����Ͽ�socket����
		*/
		bool recv_data(SocketInfo* _sockInfo)
		{
			// ���ջ�������ʣ�೤��
			int surplusLen = _sockInfo->_recvLen - (_sockInfo->_recvLast - _sockInfo->_recvStart);
			// ���ʣ�೤��������ڵ���_recv�����������򷵻�
			if (surplusLen <= 0)
				return true;
			int ret = ::recv(_sockInfo->_sock, _sockInfo->_recvLast, surplusLen, 0);
			if (-1 == ret)
			{
				return false;
			}
			else
			{
				_sockInfo->_recvLast += ret;
				return true;
			}
		}
		/**
		* @description : ��������
		* @param : SocketInfo*: ָ��SOCKET����Ϣ�ṹ��
		* @return : ʧ�ܷ���-1, �ɹ����ط��͵������ֽڴ�С
		*/
		int send_data(SocketInfo* _sockInfo)
		{
			// ���͵����ݴ�С
			int sendDataLen = _sockInfo->_sendLast - _sockInfo->_sendStart;
			return ::send(_sockInfo->_sock, _sockInfo->_sendStart, sendDataLen, 0);
		}
	public:
		SocketBase()
		{
			// ��sock����Ϊ0ʱ�ͷ�dll
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					WSADATA wsaData;
					// ����WinSock2��dll
					if (::WSAStartup(WINSOCK_VERSION, &wsaData))
						throw "WSAStartup_error";
				#endif
			}
			++_sockBaseCount;
		}
		virtual ~SocketBase()
		{
			--_sockBaseCount;
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					// �ͷ�DLL
				if (::WSACleanup())
					throw "WSACleanup_error";
				#endif
			}
		}
	};
	u_int SocketBase::_sockBaseCount = 0;
};
#endif
