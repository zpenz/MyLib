#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

#include "../UIAutomation/UIA.h"
#pragma comment(lib,"../debug/UIAutomation.lib")

using test = bool(*)(void);

int main()
{
	//test doTest;
	//auto library = LoadLibraryA("Tool.dll");
	//doTest = RPCAST(test, GetProcAddress(library, "test"));
	//doTest();
	auto root = UIA.ElementFromHwnd((HWND)0x2072E);
	auto child = UIA.FindChildElementByAID(root,"12298");
	UIA_HWND uHwnd;
	child->get_CurrentNativeWindowHandle(&uHwnd);
	auto name = UIA.GetElementName(child); 
	UIA.SetValue(child, "\\192.168");
    return 0;
}

