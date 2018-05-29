
#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

using func = bool(*)(void);
using drawline = bool(*)(int,int,int,int);


int main()
{
	drawline doFunc;
	auto library = LoadLibraryA("Tool.dll");
	doFunc = RPCAST(drawline, GetProcAddress(library, "drawLine_soft"));
	doFunc(100, 100, 50, 50);
	
	//func doFunc;
	//auto library = LoadLibraryA("LIB3D.dll");
	//doFunc = RPCAST(func,GetProcAddress(library, "DoSomething"));
	//doFunc();
    return 0;
}

