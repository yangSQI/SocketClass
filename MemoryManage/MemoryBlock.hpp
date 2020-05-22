#ifndef __MEMORYBLOCK_H__
#define __MEMORYBLOCK_H__
#include "MemoryPool.hpp"
namespace yang
{
	// 内存池中的内存块类
	class MemoryBlock
	{
	protected:
		int _nID;				// 内存块编号
		int _nRef;				// 引用计数
		MemoryPool* _pPool;		// 所属内存池
		MemoryBlock* _pNext;	// 下一块内存块位置
		bool _bPool;			// 是否在内存池中
	public:
		MemoryBlock()
		{

		}
		~MemoryBlock()
		{

		}

	};
};

#endif
