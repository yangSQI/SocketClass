#include <iostream>
#include <cstring>
#include "../MemoryManage/MemoryManage.hpp"
using namespace std;
//using namespace yang;
int main(int argc, char* argv[])
{
	int _nBlockSize;
	int nBlockSize = 19;
	int nDigit = sizeof(void*); // ��⵱ǰ����λ��
	_nBlockSize = nBlockSize % nDigit ? nBlockSize + (nDigit - nBlockSize % nDigit) : nBlockSize; // ���ڴ���С��ֵ
	printf("%d\n", _nBlockSize);
	system("pause");
	return 0;
}