#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
#include "MemoryPool.hpp"
#define MAX_MEMORY_SIZE 64
namespace yang
{
	// 内存管理类, 单例类, 调用 getInstance获取
	class MemoryManage
	{
	protected:
		MemoryManage() {
			init(0, MAX_MEMORY_SIZE, &_memPool64);
		}
		MemoryPoolTor<64, 10> _memPool64;	// 64字节的10个内存块
		MemoryPool* _szPool[MAX_MEMORY_SIZE + 1];			// 内存池映射数组
	public:
		virtual ~MemoryManage()
		{

		}
		/**
		* @description : 单例模式,获取自身
		* @return : 返回类指针
		*/
		static MemoryManage* getInstance()
		{
			static MemoryManage* _memoryManage = NULL; // 单例
			if (_memoryManage == NULL)
				_memoryManage = new MemoryManage;
			return _memoryManage;
		}
		/**
		* @description : 申请内存
		* @param : size: 申请的内存大小
		* @return : 返回申请的内存指针
		*/
		void* allocMem(size_t nSize)
		{
			if (nSize <= MAX_MEMORY_SIZE)
			{
				return _szPool[nSize]->allocMem(nSize);
			}
			else
			{
				MemoryBlock* pReturn = (MemoryBlock*)::malloc(nSize + sizeof(MemoryBlock));
				pReturn->_nID = -1;			// 内存块编号				
				pReturn->_nRef = 1;			// 内存块引用 
				pReturn->_bPool = false;	// 是否在内存 			
				pReturn->_pPool = nullptr;		// 指向内存池			
				pReturn->_pNext = nullptr;  // 内存块下一块地址为空			
				return (char*)pReturn + sizeof(MemoryBlock);
			}
		}
		/**
		* @description : 释放内存
		* @param : 释放的内存指针
		*/
		void freeMem(void* pMem)
		{
			MemoryBlock* pMemBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
			if (pMemBlock->_pPool)
			{
				pMemBlock->_pPool->freeMem(pMem);
			}
			else
			{
				if (--pMemBlock->_nRef == 0)
					::free(pMemBlock);
			}
		}
		/**
		* @description : 增加内存块的引用计数
		*/
		void addRef(void* pMem)
		{
			MemoryBlock* pMemBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
			++pMemBlock->_nRef;
		}
	protected:
		/**
		* @description : 初始化内存池映射数组
		*/
		void init(int nBegin, int nEnd, MemoryPool* pMemPool)
		{
			for (int i = nBegin; i < nEnd; ++i)
			{
				_szPool[i] = pMemPool;
			}
		}
	};
};
#endif
