#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

#include "../Tool/Inject.h"
#pragma comment(lib,"../debug/Tool.lib")

using test = bool(*)(void);

int main()
{
	InjectDLLByName(L"C:\\Users\\kefu\\Desktop\\Tool.dll", L"QQ");
    return 0;
}

