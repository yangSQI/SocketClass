#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
#include "MemoryPool.hpp"
#define MAX_MEMORY_SIZE 64
namespace yang
{
	// �ڴ������, ������, ���� getInstance��ȡ
	class MemoryManage
	{
	protected:
		MemoryManage() {
			init(0, MAX_MEMORY_SIZE, &_memPool64);
		}
		MemoryPoolTor<64, 10> _memPool64;	// 64�ֽڵ�10���ڴ��
		MemoryPool* _szPool[MAX_MEMORY_SIZE + 1];			// �ڴ��ӳ������
	public:
		virtual ~MemoryManage()
		{

		}
		/**
		* @description : ����ģʽ,��ȡ����
		* @return : ������ָ��
		*/
		static MemoryManage* getInstance()
		{
			static MemoryManage* _memoryManage = NULL; // ����
			if (_memoryManage == NULL)
				_memoryManage = new MemoryManage;
			return _memoryManage;
		}
		/**
		* @description : �����ڴ�
		* @param : size: ������ڴ��С
		* @return : ����������ڴ�ָ��
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
				pReturn->_nID = -1;			// �ڴ����				
				pReturn->_nRef = 1;			// �ڴ������ 
				pReturn->_bPool = false;	// �Ƿ����ڴ� 			
				pReturn->_pPool = nullptr;		// ָ���ڴ��			
				pReturn->_pNext = nullptr;  // �ڴ����һ���ַΪ��			
				return (char*)pReturn + sizeof(MemoryBlock);
			}
		}
		/**
		* @description : �ͷ��ڴ�
		* @param : �ͷŵ��ڴ�ָ��
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
		* @description : �����ڴ������ü���
		*/
		void addRef(void* pMem)
		{
			MemoryBlock* pMemBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
			++pMemBlock->_nRef;
		}
	protected:
		/**
		* @description : ��ʼ���ڴ��ӳ������
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
