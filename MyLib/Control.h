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
		const char * mClassName;
		void UpdatePosition(); //update position
		void UpdateCache();    //update cache
	public:
		Handle();
		bool Create();   //increase ID
		UINT GetID();
		HWND GetParent();
		void SetPosition(int x,int y);
		void SetWidth(int width);
		void SetHeight(int height);
		void AddStyle(DWORD style);
		void ReduceStyle(DWORD style);

		friend LRESULT CALLBACK ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	class View
	{
	protected:
		HWND  mHwnd;
		string mText;
		HWND  mParent;
	public:
		virtual void Draw()=0;
		bool IsVisible();
		void SetText(string st);
	};

	class Control : public Handle, public View
	{
	public:
		Control();
		virtual void OnDraw(WPARAM wParam,LPARAM lParam);
		virtual void OnClick(WPARAM wParam, LPARAM lParam);

	};
}



