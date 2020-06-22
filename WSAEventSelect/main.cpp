#include "EventSelect.h"
int main(int argc, char* argv[])
{
	EventSelect server;
	server.Init(88);
	for (;;)
	{
		server.Run();
	}
	return 0;
}