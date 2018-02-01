#pragma once
#include <vector>
#include <windows.h>
#include "Macros.h"

namespace LIB_CONTROL
{
	using namespace std;

	class Handle
	{
		static UINT ID;
	protected:
		int mX, mY,mWidth,mHeight;
		HWND  mHwnd;
		HWND  mParent;
		DWORD mStyle;
		UINT mID;
		string mClassName;
		string mText;
		void UpdatePosition(); //update position
		void UpdateCache();    //update cache
		bool Create();   //increase ID
	public:
		Handle();
		UINT GetID() const;
		HWND GetOwn() const;
		void SetPosition(int x,int y);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetProc(pCallBackFunc pRoc);
		void AddStyle(DWORD style);
		void ReduceStyle(DWORD style);
		bool AttachParent(HWND parent);
		friend LRESULT CALLBACK ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	class View
	{
	protected:
		HWND  mHwnd;
		string mText;
		HWND  mParent;
	public:
		bool IsVisible();
		bool SetVisible(bool CanSee);
		void SetText(string st);
	};

	class Control : public Handle, public View 
	{
	public:
		Control(string Type);
		Control();
		virtual void OnDraw(WPARAM wParam,LPARAM lParam);
		virtual void OnClick(WPARAM wParam, LPARAM lParam);
		virtual bool CreateObject(int x, int y,int width,int height);
	};
}



