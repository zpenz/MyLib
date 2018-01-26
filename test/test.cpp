#include "stdafx.h"
#include "../MyLib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 
	auto semphore = CreateSemaphoreW(NULL, 0, 1, L"WindowOccur");
	BaseWindow bs;
	bs.Show();
	WaitForSingleObject(semphore,INFINITE);
	cout << "成功等待出现!"<<endl;
	while (1) {}
    return 0;
}

