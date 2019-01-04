

#include "pch.h"
#include <iostream>
#include "../MyLib/lib.h"
#pragma comment(lib,"../DEBUG/lib.lib")
#ifdef _DEBUG
#pragma comment(lib,"../Debug/lib.lib")
#else
#pragma comment(lib,"../Release/lib.lib")
#endif

int main()
{
	BaseWindow bw;
	auto ThreadHandle = bw.Show();
	bw.SetWindowName(L"heiehei");
	WaitForSingleObject(ThreadHandle, INFINITE);
	CloseHandle(ThreadHandle);
}


