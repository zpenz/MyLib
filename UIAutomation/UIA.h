#pragma once
#include <UIAutomation.h>
#include <string>
#include <vector>

#ifdef UIAAPI
#define UIAAPI _declspec(dllimport)
#else 
#define UIAAPI _declspec(dllexport)
#endif 

#define UIAE  IUIAutomationElement 
#define UIAC  IUIAutomationCondition
#define UIAIP IUIAutomationInvokePattern
#define UIAUP IUIAutomationValuePattern
#define UIATW IUIAutomationTreeWalker
#define UIAEA IUIAutomationElementArray

extern "C"
{
	class UIAAPI UIAManager
	{
	private:

		IUIAutomation * m_pIUAutomation;

		static UIAManager * pInstance;

		UIAE * m_pRoot;

		UIAManager();

		bool init();

		std::vector<std::string> * Split(std::string strOldString,char splitChar = '/');
		
	public:

		HWND GetWindowByClassPath(std::string strClassPath);

		HWND GetWindowByNamePath(std::string strNamePath);

		void SetRoot(UIAE * pe) { m_pRoot = pe; }

		void SetRoot(HWND hWnd) { SetRoot(ElementFromHwnd(hWnd)); }

		static UIAManager* getInstance();

		bool ClickElement(UIAE * pae);

		bool SetForce(UIAE * pae);

		UIAE  * ElementFromHwnd(HWND hwnd);

		UIAEA * GetAllElementFromElement(UIAE * pAE, TreeScope findScope = TreeScope_Descendants);

		UIAE * GetElementByAID(std::string strAid);

		UIAE * GetElementByAIDEx(std::string strAid, UIAE * pAE);

		UIAE * GetElementByAidPath(std::string strAidPath);

		UIAIP * GetElementByCondition(UIAC * uiac);

		UIAE  * GetRoot() { return m_pRoot; }

		UIAE * GetNextSliblingElement(UIAE * pAE);

		UIAE * GetPreviousSiblingElement(UIAE * pAE);

		UIAE * GetFirstChildElement(UIAE * pAE);

		UIAE * GetFirstChildElement(HWND hWnd);

		UIAE * GetLastChildElement(UIAE * pAE);

		UIAE * GetLastChildElement(HWND hWnd);

		UIAE * FindChildElementByAID(UIAE * pAE,std::string strAID);

		UIAIP * ConvertoPattern(UIAE * pFound);

		std::string GetElementName(UIAE * pAE);

		std::string GetValue(UIAE * pAE);

		std::string GetAid(UIAE * pAE);

		bool CopyValueToElement(UIAE * pAE,std::string strValue);

		bool SetValue(UIAE * pAE, std::string strValue);

		bool Invoke(UIAE * pAE);

		~UIAManager(void);

	};

#define UIA (*UIAManager::getInstance())
}



