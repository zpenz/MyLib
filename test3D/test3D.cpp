
#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

using test = bool(*)(void);

int main()
{
	test doTest;
	auto library = LoadLibraryA("Tool.dll");
	/*doTest = RPCAST(test, GetProcAddress(library, "DoSomething"));*/
	doTest = RPCAST(test, GetProcAddress(library, "test"));
	doTest();
    return 0;
}

