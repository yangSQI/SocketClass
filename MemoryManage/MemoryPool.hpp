#ifndef __MEMORYPOOL_HPP__
#define __MEMORYPOOL_HPP__
#include <stdlib.h>
#include <assert.h>
#include "Debug.h"
namespace yang
{
	class MemoryPool;
	// �ڴ����
	class MemoryBlock
	{
	public:
		int _nID = 0;						// �ڴ����
		int _nRef = 0;						// ���ü���
		MemoryPool* _pPool = nullptr;		// �����ڴ��
		MemoryBlock* _pNext = nullptr;		// ��һ���ڴ��λ��
		bool _bPool = false;				// �Ƿ����ڴ����
	public:
		MemoryBlock()
		{

		}
		~MemoryBlock()
		{

		}

	};
	// �ڴ����
	class MemoryPool
	{
	protected:
		int _nMemoryBlockSize = sizeof(MemoryBlock); // MemoryBlock���С
	protected:
		char* _pBuf = nullptr;				// �ڴ�ص�ַ
		MemoryBlock* _pHeader = nullptr;	// �ڴ��ͷָ��
		size_t _nBlockSize = 0;				// �ڴ浥Ԫ��С,һ���ڴ��Ĵ�С
		size_t _nBlockLen = 0;				// �ڴ浥Ԫ����, ���ڴ������
	public:
		MemoryPool()
		{

		}
		~MemoryPool()
		{
			if (_pBuf != nullptr) // ����Ѿ������ڴ��
				::free(_pBuf); // �ͷ��ڴ��
		}
	public:
		/**
		* @description : �ڴ�س�ʼ��
		* @param : ��������
		* @return : ����ֵ����
		*/
		void initMemory()
		{
			if (_pBuf)
				return;
			// �����ڴ�ش�С
			size_t realSize = _nBlockSize + _nMemoryBlockSize;
			size_t bufSize = _nBlockLen * realSize;
			// Ϊ�ڴ�������ڴ�
			_pBuf = (char*)malloc(bufSize);
			_pHeader = (MemoryBlock*)_pBuf;
			MemoryBlock* pTemp = _pHeader;

			// ��ʼ���ڴ����Ϣ
			int nTemp = _nBlockLen - 1;
			for (int i = 0; i < _nBlockLen; ++i)
			{
				pTemp->_nID = i;		// �ڴ����
				pTemp->_nRef = 0;		// �ڴ�����ü���	
				pTemp->_bPool = true;	// �Ƿ����ڴ����
				pTemp->_pPool = this;   // ָ���ڴ��
				// ���һ��_pNextָ��nullptr
				if (i == nTemp)
					pTemp->_pNext = nullptr; // �ڴ����һ���ַΪ��
				else
					pTemp->_pNext = (MemoryBlock*)(_pBuf + ((i + 1) * realSize));
				pTemp = pTemp->_pNext; // �ڴ����һ���ַ
			}
		}
		/**
		* @description : �����ڴ�
		* @param : size: ������ڴ��С
		* @return : ����������ڴ�ָ��
		*/
		void* allocMem(size_t size)
		{
			// ���û�г�ʼ���ȳ�ʼ��
			if (!_pBuf)
				initMemory();
			MemoryBlock* pReturn = nullptr;
			if (nullptr == _pHeader)
			{
				pReturn = (MemoryBlock*)::malloc(_nMemoryBlockSize + size);
				pReturn->_nID = -1;			// �ڴ����				
				pReturn->_nRef = 1;			// �ڴ������ 
				pReturn->_bPool = false;	// �Ƿ����ڴ� 			
				pReturn->_pPool = nullptr;		// ָ���ڴ��			
				pReturn->_pNext = nullptr;  // �ڴ����һ���ַΪ��			
			}
			else
			{
				pReturn = _pHeader;				// �����ڴ���ַ
				_pHeader = _pHeader->_pNext;    // ָ����һ���ڴ��
				assert(0 == pReturn->_nRef);
				pReturn->_nRef = 1;
			}
			xPrintf("�����ڴ��ַ: %p, �ڴ���: %d, �ڴ��С: %u\n", pReturn, pReturn->_nID, size);
			return (char*)pReturn + _nMemoryBlockSize;
		}
		/**
		* @description : �ͷ��ڴ�
		* @param : �ͷŵ��ڴ�ָ��
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
			xPrintf("�ͷŵ��ڴ��ַ: %p, �ڴ���: %d\n", pBlock, pBlock->_nID);
		}
	};
	// �ڴ��ģ�帳ֵ,����Ա�������㸴��ģ����	
	template<size_t nBlockSize, size_t nBlockLen>
	class MemoryPoolTor : public MemoryPool
	{
	public:
		MemoryPoolTor()
		{
			int nDigit = sizeof(void*); // ��⵱ǰ����λ��
			_nBlockSize = nBlockSize % nDigit ? nBlockSize + (nDigit - nBlockSize % nDigit) : nBlockSize; // ���ڴ���С��ֵ,�Զ���ķ�ʽ
			_nBlockLen  = nBlockLen; // ���ڴ��������ֵ
		}
	};
};

#endif