#pragma once

#ifdef PSONGAPI
#define PSONGAPI _declspec(dllimport)
#else 
#define PSONGAPI _declspec(dllexport)
#endif 

#include "../MyLib/Macros.h"


extern "C"
{
	class PSONGAPI HttpClass
	{
	public:

		void test();

		void test2();

		SINGLE_INSTANCE(HttpClass)
	};

#define HttpManager HttpClass::getInstance()
}