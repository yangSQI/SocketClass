#include <iostream>
#include <ctime>
using namespace std;
int main(int argc, char* argv[])
{
	time_t _time;
	time(&_time);
	char* pTime = ctime(&_time); // ʱ�������ַ���	
	printf("%s\n", pTime);
	time_t t;
	tm* lt;
	time(&t);							//��ȡʱ�����
	lt = localtime(&t);					//תΪʱ��ṹ��
	int tm_year = lt->tm_year + 1900;	// ��ݣ���ֵ����ʵ����ݼ�ȥ1900
	int tm_mon = lt->tm_mon + 1;		    // �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
	int tm_mday = lt->tm_mday;			// һ�����е����� - ȡֵ����Ϊ[1,31] 
	int tm_hour = lt->tm_hour;			// ʱ - ȡֵ����Ϊ[0,23]
	int tm_min = lt->tm_min;			    // �� - ȡֵ����Ϊ[0,59] 
	int tm_sec = lt->tm_sec;			    // �� �C ȡֵ����Ϊ[0,59]
	char buff[100] = { 0 };
	snprintf(buff, 100, "%d %d %d %d:%d:%d\n", tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec);
	printf("%s\n", buff);
	system("pause");
	return 0;
}