#include "SocketServer.hpp"
#include "NetEventServer.hpp"
#include <iostream>
#define RECV_SIZE		10240		// 接收缓冲区字节大小
#define SEND_SIZE		10240		// 发送缓冲区字节大小
#define PORT			888			// SOCKET服务端端口号
#define CONN_NUM		128			// listen连接数
#define RECV_THREAD_NUM	2			// 接收消息的线程数量
#define SEND_THREAD_NUM 1			// 发送消息的线程数量
using namespace yang;
using namespace std;
int main(int argc, char* argv[])
{
	try
	{
		SocketServer sockServer(SocketBlockMode::BLOCK, RECV_SIZE, SEND_SIZE);
		sockServer.SetNetEvent(new NetEventServer);
		if (!sockServer.bind_and_listen(PORT, CONN_NUM))
			throw "bind_and_listen error";
		sockServer.start(RECV_THREAD_NUM, SEND_THREAD_NUM); // 启动接收和处理SOCKET的线程
		for (;;)
		{
			sockServer.onRunAccept();
		}
	}
	catch (const char* _error)
	{
	//#ifdef _DEBUG
		cout << _error << endl;
	//#endif
	}
	return 0;
}