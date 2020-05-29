#include <stdio.h>
#include <time.h>
#include "MemoryOperatorNew.hpp"
#include <thread>
#include <memory>
#include <list>
#define THREAD_NUM 8		// �߳�����
#define PERSON_NUM 100000	// Person������
using namespace yang;
using namespace std;
class Person : public MemoryOperatorNew
{
public:
	int _random;
	Person()
	{
		printf("Person���캯��\n");
		// �����ʽ: a - b(a��b)
		_random = rand() % 1001;
	}
	~Person()
	{
		printf("Person��������, _random: %d\n", _random);
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