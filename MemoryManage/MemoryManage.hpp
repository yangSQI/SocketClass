#ifndef __MEMORYMANAGE_HPP__
#define __MEMORYMANAGE_HPP__
#include <stdlib.h>
namespace yang
{
	// 内存管理类, 单例类, 调用 getInstance获取
	class MemoryManage
	{
	private:
		MemoryManage();
	protected:
		static MemoryManage* _memoryManage; // 单例

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
			if (_memoryManage == NULL)
				_memoryManage = new MemoryManage;
			return _memoryManage;
		}
		/**
		* @description : 申请内存
		* @param : size: 申请的内存大小
		* @return : 返回申请的内存指针
		*/
		void* allocMem(size_t size)
		{
			return ::malloc(size);
		}
		/**
		* @description : 释放内存
		* @param : 释放的内存指针
		*/
		void freeMem(void* ptr)
		{
			::free(p);
		}
	};
	MemoryManage* MemoryManage::_memoryManage = NULL;
};
#endif
