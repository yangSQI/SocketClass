#include <iostream>
#include <cstring>
#include "../MemoryManage/MemoryManage.hpp"
using namespace std;
//using namespace yang;
int main(int argc, char* argv[])
{
	int _nBlockSize;
	int nBlockSize = 19;
	int nDigit = sizeof(void*); // 检测当前程序位数
	_nBlockSize = nBlockSize % nDigit ? nBlockSize + (nDigit - nBlockSize % nDigit) : nBlockSize; // 给内存块大小赋值
	printf("%d\n", _nBlockSize);
	system("pause");
	return 0;
}