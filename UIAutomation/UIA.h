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
#define CALL_RESULT(hr) if(FAILED(hr)) {std::cout<<"an error happen"<<std::endl; system("pause");}else{std::cout<<"this step ok.."<<std::endl; }

#pragma comment(lib, "comsuppw.lib") 

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
	UIAE * GetElementByAIDEx(std::string strAid);
	UIAIP * GetElementByAID(std::string strAid);
	UIAIP * GetElementByCondition(UIAC * uiac);
	UIAE  * GetRoot() { return m_pRoot; }
	UIAIP * ConvertoPattern(UIAE * pFound);
	bool Invoke(UIAIP * pattern);
	~UIAManager(void);
};

