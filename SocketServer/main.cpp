#include "SocketServer.hpp"
#include <iostream>
#define RECV_SIZE	10240		// ���ջ������ֽڴ�С
#define SEND_SIZE	10240		// ���ͻ������ֽڴ�С
#define PORT		88			// SOCKET����˶˿ں�
#define CONN_NUM	128			// listen������
#define THREAD_NUM	4			// ����SOCKET���պͷ����̵߳�����
using namespace yang;
using namespace std;
int main(int argc, char* argv[])
{
	try
	{
		SocketServer sockServer(SocketBlockMode::BLOCK, RECV_SIZE, SEND_SIZE);
		if (!sockServer.bind_and_listen(PORT, CONN_NUM))
			throw "bind_and_listen error";
		sockServer.start(THREAD_NUM); // �������պʹ���SOCKET���߳�
		for (;;)
		{
			sockServer.onRunAccept();
		}
	}
	catch (const char* _error)
	{
	#ifdef _DEBUG
		cout << _error << endl;
	#endif
	}
	return 0;
}