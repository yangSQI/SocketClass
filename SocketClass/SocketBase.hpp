#ifndef __SOCKETBASE_H__
#define __SOCKETBASE_H__
	#ifdef _WIN32
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
#include <cstdio>
// SOCKET����ģʽ
namespace yang
{
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
		bool set_block_mode(SOCKET& sock, SocketBlockMode mode)
		{
			#ifdef _WIN32
				u_long blockMode = (u_long)mode;
				if (ioctlsocket(sock, FIONBIO, &blockMode)) // ����SOCKET����ģʽ
					return false;
				else
					return true;
			#else
				int blockMode = (int)mode;
				int flags = 0;
				flags = fcntl(sock, F_GETFL, 0);				//��ȡ�ļ���flagsֵ
				if (flags != 0) return false;
				if (blockMode)
					flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);	//���óɷ�����ģʽ
				else
					flags = fcntl(sock, F_SETFL, flags &~ O_NONBLOCK);	//���ó�����ģʽ
				if (flags != 0) return false;
			#endif
		}
	public:
		SocketBase()
		{
			printf("SocketBase�������\n");
			// ��sock����Ϊ0ʱ�ͷ�dll
			if (_sockBaseCount == 0)
			{
				#ifdef _WIN32
					WSADATA wsaData;
					// ����WinSock2��dll
					if (WSAStartup(WINSOCK_VERSION, &wsaData))
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
				if (WSACleanup())
					throw "WSACleanup_error";
				#endif
			}
		}
	};
};
u_int yang::SocketBase::_sockBaseCount = 0;
#endif
