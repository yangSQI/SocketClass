#ifndef __MEMORYOPERATORNEW_HPP__
#define __MEMORYOPERATORNEW_HPP__
#include "MemoryManage.hpp"
namespace yang
{
	class MemoryOperatorNew
	{
	public:
		void* operator new(size_t size)
		{
			return MemoryManage::getInstance()->allocMem(size);
		}
		void* operator new[](size_t size)
		{
			return MemoryManage::getInstance()->allocMem(size);
		}
		void operator delete(void* ptr, size_t size)
		{
			MemoryManage::getInstance()->freeMem(ptr);
		}
		void operator delete[](void* ptr, size_t size)
		{
			MemoryManage::getInstance()->freeMem(ptr);
		}
	};
};
#endif