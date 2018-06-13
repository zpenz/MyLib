#include "Inject.h"
#include <tchar.h>
#include "../MyLib/Macros.h"

bool INJECTAPI InjectDLLByID(WCHAR * szDLLPath, DWORD hProcessID)
{
	IS_RETURN_ERROR(szDLLPath==L"",false,"DLL·��Ϊ��!");
	IS_RETURN_ERROR(!hProcessID, false, "����IDΪ0!");

	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, hProcessID);
	auto dwSize = (DWORD)(_tcslen(szDLLPath) + 1) * sizeof(WCHAR);

	LPVOID pRemoteMemoryBase = VirtualAllocEx(hProcessHandle, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	IS_RETURN_ERROR(!pRemoteMemoryBase,false,"����Զ���ڴ�ʧ��!");
	auto ret = WriteProcessMemory(hProcessHandle, pRemoteMemoryBase, (LPVOID)szDLLPath, dwSize, NULL);
	IS_RETURN_ERROR(!ret, false, "Զ��д���ڴ�ʧ��!");

	HMODULE hMod = GetModuleHandle(L"kernel32.dll");
	auto pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");  
	auto hThread = CreateRemoteThread(hProcessHandle, NULL, 0, pThreadProc, pRemoteMemoryBase, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcessHandle, pRemoteMemoryBase, dwSize, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcessHandle);
	
	return true;
}

bool INJECTAPI InjectDLLByName(WCHAR * szDLLPath, WCHAR * szWindowName)
{
	auto needHwnd = FindWindow(nullptr, szWindowName);
	DWORD dProcessID = 0;
	GetWindowThreadProcessId(needHwnd, &dProcessID);
	return InjectDLLByID(szDLLPath, dProcessID);
}
