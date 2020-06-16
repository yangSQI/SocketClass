#include <iostream>
#include <ctime>
using namespace std;
int main(int argc, char* argv[])
{
	time_t _time;
	time(&_time);
	char* pTime = ctime(&_time); // 时间日期字符串	
	printf("%s\n", pTime);
	time_t t;
	tm* lt;
	time(&t);							//获取时间戳。
	lt = localtime(&t);					//转为时间结构。
	int tm_year = lt->tm_year + 1900;	// 年份，其值等于实际年份减去1900
	int tm_mon = lt->tm_mon + 1;		    // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
	int tm_mday = lt->tm_mday;			// 一个月中的日期 - 取值区间为[1,31] 
	int tm_hour = lt->tm_hour;			// 时 - 取值区间为[0,23]
	int tm_min = lt->tm_min;			    // 分 - 取值区间为[0,59] 
	int tm_sec = lt->tm_sec;			    // 秒 – 取值区间为[0,59]
	char buff[100] = { 0 };
	snprintf(buff, 100, "%d %d %d %d:%d:%d\n", tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec);
	printf("%s\n", buff);
	system("pause");
	return 0;
}