#include <iostream>
#include "SocketBase.hpp"
using namespace yang;
using namespace std;
int main(int argc, char* argv[])
{
	try
	{
		SocketBase sockBase;
	}
	catch (const char* str)
	{
		cout << str << endl;
	}
	return 0;
}