
#include "pch.h"
#include <iostream>

#include "../UIAutomation/UIA.h"
#pragma comment(lib,"../debug/UIAutomation.lib")

int main()
{
	system("Appwiz.cpl");
	auto testWindow = FindWindowA("CabinetWClass","程序和功能");
	UIA.SetRoot(testWindow);

	auto find = UIA.GetWindowByClassPath("ShellTabWindowClass/DUIViewWndClassName");
	UIA.SetRoot(find);
	auto pAE = UIA.GetElementByAidPath("main/FolderLayoutContainer");
	UIA.SetRoot(pAE);
	pAE = UIA.GetFirstChildElement(pAE);
	UIA.SetRoot(pAE);
	pAE = UIA.GetElementByAidPath("NavPanel/LinkText");

}


