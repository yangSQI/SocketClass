#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
#include "MemoryPool.hpp"
#include "Debug.h"
#include <mutex>
namespace yang
{
	// 内存管理类, 单例类, 调用 getInstance获取
	class MemoryManage
	{
	protected:
		MemoryManage() {
			initPoolMapping(0, 64, &_memPool64);
			initPoolMapping(65, 128, &_memPool128);
			initPoolMapping(129, 256, &_memPool256);
			initPoolMapping(257, 512, &_memPool512);
			initPoolMapping(513, 1024, &_memPool1024);
		}
		MemoryPoolTor<64, 100000> _memPool64;		// 64字节的10个内存块
		MemoryPoolTor<128, 100000> _memPool128;	// 128字节的10个内存块
		MemoryPoolTor<256, 100> _memPool256;	// 256字节的10个内存块
		MemoryPoolTor<512, 100> _memPool512;	// 512字节的10个内存块
		MemoryPoolTor<1024, 100> _memPool1024;	// 1024字节的10个内存块
		MemoryPool* _poolMapping[1025];			// 内存池映射数组
		std::mutex _mutex;						
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
			if (nSize <= 1024)
			{
				std::lock_guard<std::mutex> lg(_mutex); // lock_guard 在构造函数里加锁，在析构函数里解锁
				return _poolMapping[nSize]->allocMem(nSize);
			}
			else
			{
				MemoryBlock* pReturn = (MemoryBlock*)::malloc(nSize + sizeof(MemoryBlock));
				pReturn->_nID = -1;			// 内存块编号				
				pReturn->_nRef = 1;			// 内存块引用 
				pReturn->_bPool = false;	// 是否在内存 			
				pReturn->_pPool = nullptr;		// 指向内存池			
				pReturn->_pNext = nullptr;  // 内存块下一块地址为空			
				xPrintf("申请内存地址: %p, 内存编号: %d, 内存大小: %u\n", pReturn, pReturn->_nID, nSize);
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
			if (pMemBlock->_bPool)
			{
				_mutex.lock();
				pMemBlock->_pPool->freeMem(pMem);
				_mutex.unlock();
			}
			else
			{
				xPrintf("释放的内存地址: %p, 内存编号: %d\n", pMemBlock, pMemBlock->_nID);
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
		void initPoolMapping(int nBegin, int nEnd, MemoryPool* pMemPool)
		{
			for (int i = nBegin; i <= nEnd; ++i)
			{
				_poolMapping[i] = pMemPool;
			}
		}
	};
};
#endif
