#include "UIA.h"
#include <memory>
#include "../MyLib/Macros.h"

UIAManager * UIAManager::pInstance = NULL;

UIAManager * UIAManager::getInstance()
{
	if (NULL == pInstance)
	{
		pInstance = new UIAManager();
	}
	return pInstance;
}

bool UIAManager::init()
{
	m_pIUAutomation = NULL;
	m_pRoot = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIUAutomation));
	IS_RETURN_ERROR(FAILED(hr), false, "COM初始化失败!");

	hr = m_pIUAutomation->GetRootElement(&m_pRoot);
	IS_RETURN_ERROR(FAILED(hr),false,"获取根元素失败!");
	
	std::shared_ptr<void> pExit(NULL, [](void *) {
		CoUninitialize();
	});
	return true;
}

UIAE * UIAManager::GetElementByAIDEx(std::string strAid)
{
	UIAC * pCondition = NULL;
	VARIANT vt;
	vt.vt = VT_BSTR;
	vt.bstrVal = _com_util::ConvertStringToBSTR(strAid.c_str());
	CALL_RESULT(m_pIUAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vt, &pCondition));
	UIAE * pFound = NULL;
	CALL_RESULT(GetRoot()->FindFirst(TreeScope_Subtree, pCondition, &pFound));

	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");
	return pFound;
}

UIAIP * UIAManager::GetElementByAID(std::string strAid)
{
	UIAC * pCondition = NULL;
	VARIANT vt;
	vt.vt = VT_BSTR;
	vt.bstrVal = _com_util::ConvertStringToBSTR(strAid.c_str());
	CALL_RESULT(m_pIUAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vt, &pCondition));
	return GetElementByCondition(pCondition);
}

UIAIP * UIAManager::GetElementByCondition(UIAC * uiac)
{

	UIAE * pFound = NULL;
	CALL_RESULT(GetRoot()->FindFirst(TreeScope_Subtree, uiac, &pFound));
	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");

	UIAIP *  pPattern = ConvertoPattern(pFound);
	return pPattern;
}

UIAIP * UIAManager::ConvertoPattern(UIAE * pFound)
{
	UIAIP * pPattern = NULL;
	CALL_RESULT(pFound->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&pPattern)));
	return pPattern;
}

UIAE * UIAManager::GetElementByHwnd(HWND hwnd)
{
	UIAE * pe = NULL;
	UIAC * pCondition = NULL;
	m_pIUAutomation->ElementFromHandle(hwnd, &pe);
	return pe;
}

bool UIAManager::Invoke(UIAIP * pattern)
{
	if (pattern == NULL) return false;
	pattern->Invoke();
	return true;
}

bool UIAManager::SetForce(UIAE * pae)
{
	UIA_HWND hWnd;
	pae->get_CurrentNativeWindowHandle(&hWnd);
	if (hWnd)
	{
		::BringWindowToTop((HWND)hWnd);
		::SetWindowPos((HWND)hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetFocus((HWND)hWnd);
		return true;
	}
	return false;
}

bool UIAManager::ClickElement(UIAE * pUIAE)
{
	UIA_HWND uHwnd;
	pUIAE->get_CurrentNativeWindowHandle(&uHwnd);
	POINT point;
	BOOL  bClickable = false;
	SetForce(pUIAE);

	pUIAE->GetClickablePoint(&point, &bClickable);
	IS_RETURN_ERROR(!bClickable, false, "该UIAE不可以点击!");
	LPARAM lp = MAKELPARAM(point.x, point.y);
	PostMessage((HWND)uHwnd, WM_LBUTTONDOWN, VK_LBUTTON, lp);
	Sleep(50);
	PostMessage((HWND)uHwnd, WM_LBUTTONUP, 0, lp);
	return true;
}

UIAManager::UIAManager(void)
{
	IS_RETURN_ERROR(!init(), , "构造UIAManager失败!");
}


UIAManager::~UIAManager(void)
{
	//CoUninitialize();
}


