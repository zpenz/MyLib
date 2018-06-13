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
	auto hWnd = ::FindWindow(nullptr,L"ÔËÐÐ");
	auto root = UIA.ElementFromHwnd(hWnd);
	auto child = UIA.FindChildElementByAID(root,"12298");
	auto Edit = UIA.FindChildElementByAID(child,"1001");
	UIA.SetValue(Edit, "\\\\192.168.1.188");
	auto strValue = UIA.GetValue(Edit);
	auto RightButton = UIA.FindChildElementByAID(root, "1");
	UIA.Invoke(RightButton);
	auto CancelButton = UIA.FindChildElementByAID(root, "2");
	auto name = UIA.GetElementName(child); 
    return 0;
}

