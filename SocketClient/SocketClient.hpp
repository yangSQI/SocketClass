#ifndef __SOCKETCLIENT_HPP__
#define __SOCKETCLIENT_HPP__
#include "../SocketClass/SocketBase.hpp"
namespace yang
{
	class SocketClient :
		public SocketBase
	{
	public:
		SOCKET sockServer = 0;
		sockaddr_in addrServer = { 0 };
	private:
	public:
		SocketClient(int port)
		{
			sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			addrServer.sin_family = AF_INET;
			addrServer.sin_port = htons(port);
			if (inet_pton(AF_INET, "127.0.0.1", (void*)&addrServer.sin_addr.s_addr) <= 0)
				return;
			connect(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));
		}
		void SendData(const char* data, int dataLen)
		{
			::send(sockServer, data, dataLen, 0);
		}
	};
};

#endif
