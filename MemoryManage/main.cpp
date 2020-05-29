#include <stdio.h>
#include <time.h>
#include "MemoryOperatorNew.hpp"
#include <thread>
#include <memory>
#include <list>
#define THREAD_NUM 8		// 线程数量
#define PERSON_NUM 100000	// Person类数量
using namespace yang;
using namespace std;
class Person : public MemoryOperatorNew
{
public:
	int _random;
	Person()
	{
		printf("Person构造函数\n");
		// 随机公式: a - b(a到b)
		_random = rand() % 1001;
	}
	~Person()
	{
		printf("Person析构函数, _random: %d\n", _random);
	}
};
int main(int argc, char* argv[])
{
	srand(time(NULL));
	list<unique_ptr<Person>> _list;
	_list.push_back(make_unique<Person>());
	_list.push_back(make_unique<Person>());
	_list.push_back(make_unique<Person>());
	_list.erase(_list.begin());
	system("pause");
	return 0;
}