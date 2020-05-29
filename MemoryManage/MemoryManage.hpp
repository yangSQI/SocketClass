#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
#include "MemoryPool.hpp"
#include "Debug.h"
#include <mutex>
namespace yang
{
	// �ڴ������, ������, ���� getInstance��ȡ
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
		MemoryPoolTor<64, 100000> _memPool64;		// 64�ֽڵ�10���ڴ��
		MemoryPoolTor<128, 100000> _memPool128;	// 128�ֽڵ�10���ڴ��
		MemoryPoolTor<256, 100> _memPool256;	// 256�ֽڵ�10���ڴ��
		MemoryPoolTor<512, 100> _memPool512;	// 512�ֽڵ�10���ڴ��
		MemoryPoolTor<1024, 100> _memPool1024;	// 1024�ֽڵ�10���ڴ��
		MemoryPool* _poolMapping[1025];			// �ڴ��ӳ������
		std::mutex _mutex;						
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
			if (nSize <= 1024)
			{
				std::lock_guard<std::mutex> lg(_mutex); // lock_guard �ڹ��캯������������������������
				return _poolMapping[nSize]->allocMem(nSize);
			}
			else
			{
				MemoryBlock* pReturn = (MemoryBlock*)::malloc(nSize + sizeof(MemoryBlock));
				pReturn->_nID = -1;			// �ڴ����				
				pReturn->_nRef = 1;			// �ڴ������ 
				pReturn->_bPool = false;	// �Ƿ����ڴ� 			
				pReturn->_pPool = nullptr;		// ָ���ڴ��			
				pReturn->_pNext = nullptr;  // �ڴ����һ���ַΪ��			
				xPrintf("�����ڴ��ַ: %p, �ڴ���: %d, �ڴ��С: %u\n", pReturn, pReturn->_nID, nSize);
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
			if (pMemBlock->_bPool)
			{
				_mutex.lock();
				pMemBlock->_pPool->freeMem(pMem);
				_mutex.unlock();
			}
			else
			{
				xPrintf("�ͷŵ��ڴ��ַ: %p, �ڴ���: %d\n", pMemBlock, pMemBlock->_nID);
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
