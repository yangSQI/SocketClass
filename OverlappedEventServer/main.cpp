// overlapped(�¼�ѡ��ʽ)
#include "OverlappedEventServer.h"
int main(int argc, char* argv[])
{
	OverlappedEventServer server;
	if (!server.Init(88)) return -1;
	for (;;)
	{
		server.Run();
	}
	return 0;
}