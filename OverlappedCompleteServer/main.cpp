#include "OverlappedCompleteServer.h"
int main(int argc, char* argv[])
{
	OverlappedCompleteServer server;
	server.Init(88);
	for (;;)
	{
		server.Run();
	}
	return 0;
}