#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__
#include "MemoryBlock.hpp"
namespace yang
{
	// 内存池类
	class MemoryPool
	{
	protected:
		char* _pBuf = NULL;					// 内存池地址
		MemoryBlock* _pHeader = NULL;		// 内存块头指针
		size_t _nSize = 0;					// 内存单元大小,即内存池大小
		size_t _nBlockSize = 0;				// 内存池中的内存块数量
	public:
		MemoryPool()
		{

		}
		~MemoryPool()
		{

		}
	public:
		/**
		* @description : 内存池初始化
		* @param : 参数描述
		* @return : 返回值描述
		*/
		void initMemory()
		{
			// 为内存池申请内存

			// 初始化内存块信息
		}
		/**
		* @description : 申请内存
		* @param : size: 申请的内存大小
		* @return : 返回申请的内存指针
		*/
		void* allocMem(size_t size)
		{

		}
		/**
		* @description : 释放内存
		* @param : 释放的内存指针
		*/
		void freeMem(void* ptr)
		{

		}
	};
	
};

#endif