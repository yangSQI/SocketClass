#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__
#include "MemoryBlock.hpp"
namespace yang
{
	// �ڴ����
	class MemoryPool
	{
	protected:
		char* _pBuf = NULL;					// �ڴ�ص�ַ
		MemoryBlock* _pHeader = NULL;		// �ڴ��ͷָ��
		size_t _nSize = 0;					// �ڴ浥Ԫ��С,���ڴ�ش�С
		size_t _nBlockSize = 0;				// �ڴ���е��ڴ������
	public:
		MemoryPool()
		{

		}
		~MemoryPool()
		{

		}
	public:
		/**
		* @description : �ڴ�س�ʼ��
		* @param : ��������
		* @return : ����ֵ����
		*/
		void initMemory()
		{
			// Ϊ�ڴ�������ڴ�

			// ��ʼ���ڴ����Ϣ
		}
		/**
		* @description : �����ڴ�
		* @param : size: ������ڴ��С
		* @return : ����������ڴ�ָ��
		*/
		void* allocMem(size_t size)
		{

		}
		/**
		* @description : �ͷ��ڴ�
		* @param : �ͷŵ��ڴ�ָ��
		*/
		void freeMem(void* ptr)
		{

		}
	};
	
};

#endif