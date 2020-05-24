#include <stdio.h>
#include "MemoryOperatorNew.hpp"
using namespace yang;
class Person : public MemoryOperatorNew
{
public:
};
int main(int argc, char* argv[])
{
	Person* _person1 = new Person;
	delete _person1;
	return 0;
}