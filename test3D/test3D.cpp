#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

#include "../UIAutomation/UIA.h"
#pragma comment(lib,"../debug/UIAutomation.lib")

using test = bool(*)(void);

using test2 = int(*)(int, int,DWORD * pt);

int main()
{
	auto hWnd = FindWindow(NULL,L"无标题 - 记事本"); 
	LPDWORD lpProcessId = nullptr;
	::GetWindowThreadProcessId(hWnd,lpProcessId);

	HANDLE hProceeHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 17528);

	LPCTSTR szDllPath = L"D:\\software\\rep\\MyLib\\Debug\\Tool.dll";
	auto dwSize = (DWORD)(_tcslen(szDllPath)+1)*sizeof(WCHAR);

	LPVOID pRemoteThread = VirtualAllocEx(hProceeHandle,NULL, dwSize,MEM_COMMIT,PAGE_READWRITE);
	auto ret = WriteProcessMemory(hProceeHandle, pRemoteThread,(LPVOID)szDllPath,dwSize,NULL);
	auto error = GetLastError();

	HMODULE hMod = GetModuleHandle(L"kernel32.dll");
	auto pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");  // 获得LoadLibrary()函数的地址  

	auto hThread = CreateRemoteThread(hProceeHandle, NULL, 0, pThreadProc, pRemoteThread, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProceeHandle, pRemoteThread, dwSize, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProceeHandle);

	test2 streamLogin;
	auto library = LoadLibraryA("D:\\steam\\Steam.dll");
	streamLogin = RPCAST(test2, GetProcAddress(library,"SteamChangeAccountName"));
	DWORD a = 1;
	ret = streamLogin(1,1,&a);


	//test doTest;
	//auto library = LoadLibraryA("Tool.dll");
	//doTest = RPCAST(test, GetProcAddress(library, "test"));
	//doTest();
	 hWnd = ::FindWindow(nullptr,L"运行");
	auto root = UIA.ElementFromHwnd(hWnd);
	auto child = UIA.FindChildElementByAID(root,"12298");
	auto Edit = UIA.FindChildElementByAID(child,"1001");
	//UIA.SetValue(Edit, "\\\\192.168.1.188");
	UIA.CopyValueToElement(Edit,"\\\\192.168.1.188");
	auto strValue = UIA.GetValue(Edit);
	auto RightButton = UIA.FindChildElementByAID(root, "1");
	UIA.Invoke(RightButton);
	auto CancelButton = UIA.FindChildElementByAID(root, "2");
	auto name = UIA.GetElementName(child); 
    return 0;
}

