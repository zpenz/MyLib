#pragma once
#include <windows.h>

#ifdef INJECTAPI
#define INJECTAPI _declspec(dllimport)
#else 
#define INJECTAPI _declspec(dllexport)
#endif 

extern "C"
{
	bool INJECTAPI InjectDLLByID(WCHAR * szDLLPath, DWORD hProcessID);

	bool INJECTAPI InjectDLLByName(WCHAR * szDLLPath, WCHAR * szWindowName);
}
