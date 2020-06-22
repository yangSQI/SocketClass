#include <iostream>
#include "SocketClient.hpp"
#include <thread>
#include <chrono>
using namespace std;
using namespace yang;
int main(int argc, char* argv[])
{
	SocketClient sockClient(88);
	char buff[1024] = { 0 };
	strcpy(buff, "El Psy Congroo!ËFµ¿");
	std::chrono::seconds t(1);
	for (;;)
	{ 
		getchar();
		sockClient.SendData(buff, 1024);
		// —” ±
		std::this_thread::sleep_for(t);
	}
	return 0;
}