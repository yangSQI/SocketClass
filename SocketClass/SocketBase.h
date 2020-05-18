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
		bool set_block_mode(SOCKET& sock, SocketBlockMode mode);
	public:
		SocketBase();
		virtual ~SocketBase();
	};
};
#endif
