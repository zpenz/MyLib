#pragma once
#include <UIAutomation.h>
#include <vector>
#include <iostream>
#include <string>
#include <comutil.h>

#define UIA   UIAManager::getInstance()
#define UIAE  IUIAutomationElement 
#define UIAC  IUIAutomationCondition
#define UIAIP IUIAutomationInvokePattern
#define UIATW IUIAutomationTreeWalker

class UIAManager
{
private:

	IUIAutomation * m_pIUAutomation;

	std::vector<IUIAutomationElement *> m_sElement;

	static UIAManager * pInstance;

	UIAE * m_pRoot;

	UIAManager();

	bool init();

public:
	void SetRoot(UIAE * pe) { m_pRoot = pe; }

	static UIAManager* getInstance();

	bool ClickElement(UIAE * pae);

	bool SetForce(UIAE * pae);

	UIAE  * GetElementByHwnd(HWND hwnd);

	UIAE * GetElementByAID(std::string strAid);

	UIAIP * GetElementByCondition(UIAC * uiac);

	UIAE  * GetRoot() { return m_pRoot; }

	UIAE * GetNextSliblingElement(UIAE * pAE);

	UIAE * GetPreviousSiblingElement(UIAE * pAE);

	UIAIP * ConvertoPattern(UIAE * pFound);

	std::string GetElementName(UIAE * pAE);

	bool SetValue(UIAE * pAE,std::string strValue);

	bool Invoke(UIAE * pAE);

	~UIAManager(void);
};

