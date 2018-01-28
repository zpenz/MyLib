#include "Control.h"
#include <algorithm>

namespace LIB_CONTROL
{
	UINT Handle::ID = 678 + 1;

	void Handle::UpdatePosition()
	{
		MoveWindow(mHwnd, mX, mY, mWidth, mHeight, true);
	}
	void Handle::UpdateCache()
	{
		SetWindowPos(mHwnd,HWND_TOP,mX,mY,mWidth,
			mHeight,SWP_FRAMECHANGED);
	}

	Handle::Handle():mX(0),mY(0),mWidth(),mHeight(),mStyle(WS_CHILD|WS_VISIBLE)
	{

	}

	bool Handle::Create()
	{
		mID = ID++;
		mHwnd = CreateWindow(mClassName, "", mStyle, mX, mY, 
			mWidth, mHeight, NULL, HMENU(mID), NULL,NULL);
		if (!mHwnd) return false;
		return true;
	}

	UINT Handle::GetID()
	{
		return mID;
	}

	HWND Handle::GetParent()
	{
		return mParent;
	}

	void Handle::SetPosition(int x, int y)
	{
		mX = x, mY = y;
		if (mHwnd) UpdatePosition();
	}

	void Handle::SetWidth(int width)
	{
		mWidth = width;
		if (mHwnd) UpdatePosition();
	}

	void Handle::SetHeight(int height)
	{
		mHeight = height;
		if (mHwnd) UpdatePosition();
	}

	void Handle::AddStyle(DWORD style)
	{
		if (mHwnd)
		{
			mStyle = mStyle | style;
			auto tempStyle = GetWindowLong(mHwnd,GWL_STYLE);
			SetWindowLong(mHwnd,GWL_STYLE,tempStyle|style);
			UpdateCache();
		}
	}

	void Handle::ReduceStyle(DWORD style)
	{
		if (mHwnd)
		{
			mStyle = mStyle & ~style;
			auto tempStyle = GetWindowLong(mHwnd, GWL_STYLE);
			SetWindowLong(mHwnd, GWL_STYLE, tempStyle &  ~style);
			UpdateCache();
		}
	}


	LRESULT CALLBACK ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DRAWITEM:
		case WM_COMMAND:

		default:
			break;
		}
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}


	bool View::IsVisible()
	{
		if (!mHwnd) return false;
		if (GetWindowLong(mHwnd, GWL_STYLE)&WS_VISIBLE) return true;
		return false;
	}

	void View::SetText(string st)
	{
		if (mHwnd) SetWindowText(mHwnd, st.c_str());
	}


	Control::Control():Handle()
	{

	}

	void Control::OnDraw(WPARAM wParam, LPARAM lParam)
	{

	}
	void Control::OnClick(WPARAM wParam, LPARAM lParam)
	{

	}

}
