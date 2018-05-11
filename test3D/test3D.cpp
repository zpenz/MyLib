

#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

typedef bool(*func)(void);

int main()
{
	func doFunc;
	auto library = LoadLibraryA("LIB3D.dll");
	doFunc = RPCAST(func,GetProcAddress(library, "DoSomething"));
	auto errorCode = GetLastError();
	doFunc();
    return 0;
}

