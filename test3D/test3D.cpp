#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

using test = bool(*)(void);

#include "../Http/Http.h"
#pragma comment(lib,"../debug/Http.lib")

int main()
{
	HttpManager.test2();
    return 0;
}

