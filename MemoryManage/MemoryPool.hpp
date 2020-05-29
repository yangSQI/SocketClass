#ifndef __MEMORYPOOL_HPP__
#define __MEMORYPOOL_HPP__
#include <stdlib.h>
#include <assert.h>
#include "Debug.h"
namespace yang
{
	class MemoryPool;
	// 内存块类
	class MemoryBlock
	{
	public:
		int _nID = 0;						// 内存块编号
		int _nRef = 0;						// 引用计数
		MemoryPool* _pPool = nullptr;		// 所属内存池
		MemoryBlock* _pNext = nullptr;		// 下一块内存块位置
		bool _bPool = false;				// 是否在内存池中
	public:
		MemoryBlock()
		{

		}
		~MemoryBlock()
		{

		}

	};
	// 内存池类
	class MemoryPool
	{
	protected:
		int _nMemoryBlockSize = sizeof(MemoryBlock); // MemoryBlock类大小
	protected:
		char* _pBuf = nullptr;				// 内存池地址
		MemoryBlock* _pHeader = nullptr;	// 内存块头指针
		size_t _nBlockSize = 0;				// 内存单元大小,一个内存块的大小
		size_t _nBlockLen = 0;				// 内存单元数量, 即内存块数量
	public:
		MemoryPool()
		{

		}
		~MemoryPool()
		{
			if (_pBuf != nullptr) // 如果已经申请内存池
				::free(_pBuf); // 释放内存池
		}
	public:
		/**
		* @description : 内存池初始化
		* @param : 参数描述
		* @return : 返回值描述
		*/
		void initMemory()
		{
			if (_pBuf)
				return;
			// 计算内存池大小
			size_t realSize = _nBlockSize + _nMemoryBlockSize;
			size_t bufSize = _nBlockLen * realSize;
			// 为内存池申请内存
			_pBuf = (char*)malloc(bufSize);
			_pHeader = (MemoryBlock*)_pBuf;
			MemoryBlock* pTemp = _pHeader;

			// 初始化内存块信息
			int nTemp = _nBlockLen - 1;
			for (int i = 0; i < _nBlockLen; ++i)
			{
				pTemp->_nID = i;		// 内存块编号
				pTemp->_nRef = 0;		// 内存块引用计数	
				pTemp->_bPool = true;	// 是否在内存池中
				pTemp->_pPool = this;   // 指向内存池
				// 最后一个_pNext指向nullptr
				if (i == nTemp)
					pTemp->_pNext = nullptr; // 内存块下一块地址为空
				else
					pTemp->_pNext = (MemoryBlock*)(_pBuf + ((i + 1) * realSize));
				pTemp = pTemp->_pNext; // 内存块下一块地址
			}
		}
		/**
		* @description : 申请内存
		* @param : size: 申请的内存大小
		* @return : 返回申请的内存指针
		*/
		void* allocMem(size_t size)
		{
			// 如果没有初始化先初始化
			if (!_pBuf)
				initMemory();
			MemoryBlock* pReturn = nullptr;
			if (nullptr == _pHeader)
			{
				pReturn = (MemoryBlock*)::malloc(_nMemoryBlockSize + size);
				pReturn->_nID = -1;			// 内存块编号				
				pReturn->_nRef = 1;			// 内存块引用 
				pReturn->_bPool = false;	// 是否在内存 			
				pReturn->_pPool = nullptr;		// 指向内存池			
				pReturn->_pNext = nullptr;  // 内存块下一块地址为空			
			}
			else
			{
				pReturn = _pHeader;				// 返回内存块地址
				_pHeader = _pHeader->_pNext;    // 指向下一块内存块
				assert(0 == pReturn->_nRef);
				pReturn->_nRef = 1;
			}
			xPrintf("申请内存地址: %p, 内存编号: %d, 内存大小: %u\n", pReturn, pReturn->_nID, size);
			return (char*)pReturn + _nMemoryBlockSize;
		}
		/**
		* @description : 释放内存
		* @param : 释放的内存指针
		*/
		void freeMem(void* ptr)
		{
			MemoryBlock* pBlock = (MemoryBlock*)((char*)ptr - _nMemoryBlockSize);
			assert(1 == pBlock->_nRef);
			if (--pBlock->_nRef != 0)
			{
				return;
			}
			if (pBlock->_bPool)
			{
				pBlock->_pNext = _pHeader;
				_pHeader = pBlock;
			}
			else
			{
				::free(pBlock);
			}
			xPrintf("释放的内存地址: %p, 内存编号: %d\n", pBlock, pBlock->_nID);
		}
	};
	// 内存池模板赋值,给成员变量方便复制模板类	
	template<size_t nBlockSize, size_t nBlockLen>
	class MemoryPoolTor : public MemoryPool
	{
	public:
		MemoryPoolTor()
		{
			int nDigit = sizeof(void*); // 检测当前程序位数
			_nBlockSize = nBlockSize % nDigit ? nBlockSize + (nDigit - nBlockSize % nDigit) : nBlockSize; // 给内存块大小赋值,以对齐的方式
			_nBlockLen  = nBlockLen; // 给内存块数量赋值
		}
	};
};

#endif