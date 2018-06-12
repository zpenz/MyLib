#pragma once
#include <UIAutomation.h>
#include <string>

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

	public:
		void SetRoot(UIAE * pe) { m_pRoot = pe; }

		static UIAManager* getInstance();

		bool ClickElement(UIAE * pae);

		bool SetForce(UIAE * pae);

		UIAE  * ElementFromHwnd(HWND hwnd);

		UIAE * GetElementByAID(std::string strAid);

		UIAIP * GetElementByCondition(UIAC * uiac);

		UIAE  * GetRoot() { return m_pRoot; }

		UIAE * GetNextSliblingElement(UIAE * pAE);

		UIAE * GetPreviousSiblingElement(UIAE * pAE);

		UIAE * GetFirstChildElement(UIAE * pAE);

		UIAE * GetLastChildElement(UIAE * pAE);

		UIAE * FindChildElementByAID(UIAE * pAE,std::string strAID);

		UIAIP * ConvertoPattern(UIAE * pFound);

		std::string GetElementName(UIAE * pAE);

		bool SetValue(UIAE * pAE, std::string strValue);

		bool Invoke(UIAE * pAE);

		~UIAManager(void);
	};

#define UIA (*UIAManager::getInstance())
}



