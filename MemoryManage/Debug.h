#ifndef __DEBUG_H__
#define __DEBUG_H__
	#ifdef _DEBUG
	#include <stdio.h>
	#define xPrintf(...) printf(__VA_ARGS__) 
	//#define xPrintf(...)
	#else
	#define xPrintf(...)
	#endif
#endif