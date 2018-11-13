#include "UIA.h"
#include <memory>
#include "../MyLib/Macros.h"
#include <comutil.h>

#pragma comment(lib,"../debug/lib.lib")
#pragma comment(lib, "comsuppw.lib")

UIAManager * UIAManager::pInstance = NULL;

using namespace std;

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
	
	return true;
}

UIAE * UIAManager::GetElementByAID(std::string strAid)
{
	UIAC * pCondition = NULL;
	VARIANT vt;
	vt.vt = VT_BSTR;
	vt.bstrVal = _com_util::ConvertStringToBSTR(strAid.c_str());
	IS_FAILED_ERROR(m_pIUAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vt, &pCondition),nullptr,"创建UIA条件失败");
	UIAE * pFound = NULL;
	IS_FAILED(GetRoot()->FindFirst(TreeScope_Subtree, pCondition, &pFound),nullptr);

	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");
	return pFound;
}


UIAE * UIAManager::GetElementByAIDEx(std::string strAid, UIAE * pAE)
{
	IS_RETURN_ERROR(!pAE,nullptr,"GetElementByAIDEx pAE为空!");
	UIAC * pCondition = NULL;
	VARIANT vt;
	vt.vt = VT_BSTR;
	vt.bstrVal = _com_util::ConvertStringToBSTR(strAid.c_str());
	IS_FAILED_ERROR(m_pIUAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vt, &pCondition), nullptr, "创建UIA条件失败");
	UIAE * pFound = NULL;
	IS_FAILED(pAE->FindFirst(TreeScope_Subtree, pCondition, &pFound), nullptr);

	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");
	return pFound;
}


UIAE * UIAManager::GetElementByAidPath(std::string strAidPath)
{
	auto pList = this->Split(strAidPath);
	UIAE * pFound = GetElementByAID(*pList->begin());
	for (auto it = pList->begin()+1; it != pList->end(); it++) {
		pFound = GetElementByAIDEx(*it,pFound);
		if (!pFound) break;
	}
	return pFound;
}

UIAIP * UIAManager::GetElementByCondition(UIAC * uiac)
{
	UIAE * pFound = NULL;
	IS_FAILED(GetRoot()->FindFirst(TreeScope_Subtree, uiac, &pFound),nullptr);
	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");

	UIAIP *  pPattern = ConvertoPattern(pFound);
	return pPattern;
}

UIAE * UIAManager::GetNextSliblingElement(UIAE * pAE)
{
	UIATW * pTW = nullptr;
	IS_RETURN_ERROR(!pAE, nullptr, "UIA元素为空");
	UIAC * pAC = nullptr;
	m_pIUAutomation->CreateTrueCondition(&pAC);
	m_pIUAutomation->CreateTreeWalker(pAC, &pTW);
	UIAE * pRE = nullptr;
	pTW->GetNextSiblingElement(pAE, &pRE);
	return pRE;
}

UIAE * UIAManager::GetPreviousSiblingElement(UIAE * pAE)
{
	UIATW * pTW = nullptr;
	IS_RETURN_ERROR(!pAE, nullptr, "UIA元素为空");
	UIAC * pAC = nullptr;
	m_pIUAutomation->CreateTrueCondition(&pAC);
	m_pIUAutomation->CreateTreeWalker(pAC, &pTW);
	UIAE * pRE = nullptr;
	pTW->GetPreviousSiblingElement(pAE, &pRE);
	return pRE;
}

UIAE * UIAManager::GetFirstChildElement(UIAE * pAE)
{
	UIATW * pTW = nullptr;
	IS_RETURN_ERROR(!pAE, nullptr, "UIA元素为空");
	UIAC * pAC = nullptr;
	m_pIUAutomation->CreateTrueCondition(&pAC);
	m_pIUAutomation->CreateTreeWalker(pAC, &pTW);
	UIAE * pRE = nullptr;
	pTW->GetFirstChildElement(pAE, &pRE);
	return pRE;
}

UIAE * UIAManager::GetLastChildElement(UIAE * pAE)
{
	UIATW * pTW = nullptr;
	IS_RETURN_ERROR(!pAE, nullptr, "UIA元素为空");
	UIAC * pAC = nullptr;
	m_pIUAutomation->CreateTrueCondition(&pAC);
	m_pIUAutomation->CreateTreeWalker(pAC, &pTW);
	UIAE * pRE = nullptr;
	pTW->GetLastChildElement(pAE, &pRE);
	return pRE;
}

UIAE * UIAManager::FindChildElementByAID(UIAE * pAE, std::string strAID)
{
	IS_RETURN(!pAE, nullptr);
	UIAC * pCondition = NULL;
	VARIANT vt;
	vt.vt = VT_BSTR;
	vt.bstrVal = _com_util::ConvertStringToBSTR(strAID.c_str());
	IS_FAILED_ERROR(m_pIUAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vt, &pCondition), nullptr, "创建UIA条件失败");
	UIAE * pFound = NULL;
	IS_FAILED(pAE->FindFirst(TreeScope_Subtree, pCondition, &pFound), nullptr);

	IS_RETURN_ERROR(!pFound, nullptr, "找不到UIAE元素");
	return pFound;
}

UIAIP * UIAManager::ConvertoPattern(UIAE * pAE)
{
	IS_RETURN(!pAE, nullptr);
	UIAIP * pPattern = NULL;
	IS_FAILED(pAE->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&pPattern)), nullptr);
	return pPattern;
}

std::string UIAManager::GetElementName(UIAE * pAE)
{
	IS_RETURN(!pAE, "");
	BSTR name;
	pAE->get_CurrentName(&name);
	return _com_util::ConvertBSTRToString(name);
}

std::string UIAManager::GetValue(UIAE * pAE)
{
	IS_RETURN(!pAE, "");
	UIAUP * pPattern = nullptr;
	IS_FAILED(pAE->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&pPattern)), nullptr);
	BSTR  pRet;
	IS_RETURN(!pPattern, "");
	pPattern->get_CurrentValue(&pRet);
	return _com_util::ConvertBSTRToString(pRet);
}

bool UIAManager::CopyValueToElement(UIAE * pAE,std::string strValue)
{
	IS_RETURN(!pAE, false);
	IS_FAILED_ERROR(::OpenClipboard(NULL),false,"打开剪切板失败!");
	::EmptyClipboard();
	auto Clip = ::GlobalAlloc(GMEM_MOVEABLE, strValue.length() + 1);
	char * pbuf = (char *)GlobalLock(Clip);
	strcpy(pbuf,strValue.c_str());
	GlobalUnlock(Clip);
	SetClipboardData(CF_TEXT, Clip);
	CloseClipboard();
	 
	UIA_HWND uHwnd;
	pAE->get_CurrentNativeWindowHandle(&uHwnd);
	::SendMessage((HWND)uHwnd, WM_PASTE, 0, 0);

	OpenClipboard(NULL);
	EmptyClipboard();
	CloseClipboard();
	return true;
}

bool UIAManager::SetValue(UIAE * pAE,std::string strValue)
{
	IS_RETURN(!pAE, false);
	UIAUP * pPattern = NULL;
	IS_FAILED(pAE->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&pPattern)), nullptr);
	pPattern->SetValue(_com_util::ConvertStringToBSTR(strValue.c_str()));
	return false;
}

UIAE * UIAManager::ElementFromHwnd(HWND hwnd)
{
	UIAE * pe = NULL;
	UIAC * pCondition = NULL;
	m_pIUAutomation->ElementFromHandle(hwnd, &pe);
	return pe;
}

bool UIAManager::Invoke(UIAE * pAE)
{
	IS_RETURN_ERROR(!pAE,false,"UIA元素为空");
	ConvertoPattern(pAE)->Invoke();
	return true;
}

bool UIAManager::SetForce(UIAE * pAE)
{
	IS_RETURN(!pAE, false);
	UIA_HWND hWnd;
	pAE->get_CurrentNativeWindowHandle(&hWnd);
	IS_RETURN(!hWnd, false);

	::BringWindowToTop((HWND)hWnd);
	::SetWindowPos((HWND)hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	::SetFocus((HWND)hWnd);

	return true;
}

bool UIAManager::ClickElement(UIAE * pUIAE)
{
	UIA_HWND uHwnd;
	pUIAE->get_CurrentNativeWindowHandle(&uHwnd);
	POINT point;
	BOOL  bClickable = false;
	SetForce(pUIAE);

	pUIAE->GetClickablePoint(&point, &bClickable);
	if (!bClickable)
	{
		RECT boundRectangle;
		pUIAE->get_CurrentBoundingRectangle(&boundRectangle);
		Conver::ClientToScreenRc((HWND)uHwnd,boundRectangle);
		auto centerPoint = Conver::CenterPoint(boundRectangle);
		INPUT input[3];
		input[0].type = INPUT_MOUSE;
		input[1].type = INPUT_MOUSE;
		input[2].type = INPUT_MOUSE;
		input[0].mi.dx = centerPoint.x;
		input[0].mi.dy = centerPoint.y;
		input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
		input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(3, input, sizeof(INPUT));
		return true;
	}
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
	CoUninitialize();
}

std::vector<std::string> * UIAManager::Split(std::string strOldString, char splitChar /*= '/'*/)
{
	std::vector<std::string> * pRet = new std::vector<std::string>();
	std::string strTempValue = "";
	for (int index = 0; index < strOldString.length(); index++) {
		if (strOldString.c_str()[index] == splitChar) {
			pRet->push_back(strTempValue);
			strTempValue = "";
			continue;
		}
		strTempValue += strOldString.c_str()[index];
	}

	pRet->push_back(strTempValue);
	return pRet;
}


