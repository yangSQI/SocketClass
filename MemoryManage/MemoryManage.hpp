#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
namespace yang
{
	// �ڴ������, ������, ���� getInstance��ȡ
	class MemoryManage
	{
	private:
		MemoryManage();
	protected:
		static MemoryManage* _memoryManage; // ����

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
			if (_memoryManage == NULL)
				_memoryManage = new MemoryManage;
			return _memoryManage;
		}
		/**
		* @description : �����ڴ�
		* @param : size: ������ڴ��С
		* @return : ����������ڴ�ָ��
		*/
		void* allocMem(size_t size)
		{
			return ::malloc(size);
		}
		/**
		* @description : �ͷ��ڴ�
		* @param : �ͷŵ��ڴ�ָ��
		*/
		void freeMem(void* ptr)
		{
			::free(p);
		}
	};
	MemoryManage* MemoryManage::_memoryManage = NULL;
};
#endif
