#ifndef __MEMORYBLOCK_H__
#define __MEMORYBLOCK_H__
#include "MemoryPool.hpp"
namespace yang
{
	// �ڴ���е��ڴ����
	class MemoryBlock
	{
	protected:
		int _nID;				// �ڴ����
		int _nRef;				// ���ü���
		MemoryPool* _pPool;		// �����ڴ��
		MemoryBlock* _pNext;	// ��һ���ڴ��λ��
		bool _bPool;			// �Ƿ����ڴ����
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
