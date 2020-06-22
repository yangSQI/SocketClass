#include "IocpServer.h"
int main(int argc, char* argv[])
{
	IocpServer server;
	if (!server.Init(88)) return -1;
	for (;;)
	{
		server.Run();
	}
	return 0;
}