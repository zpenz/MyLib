#include "stdafx.h"
#include "Macros.h"
#include <d2d1.h>

void ErrorMessage(const char * _error)
{
	MessageBox(NULL, _error, "error", MB_OK);
	exit(0);
}

namespace Conver
{

	D2D1_POINT_2F & PointToD2DPointF(POINT & pt)
	{
		D2D1_POINT_2F *pPointF = new D2D1_POINT_2F(
			D2D1::Point2F(static_cast<float>(pt.x),
				static_cast<float>(pt.y)));
		return *pPointF;
	}

	D2D1_RECT_F & RectToD2DRectF(RECT & rc)
	{
		D2D1_RECT_F * pNeedRect = new D2D1_RECT_F(
			D2D1::RectF(static_cast<float>(rc.left),
				static_cast<float>(rc.top),
				static_cast<float>(rc.right),
				static_cast<float>(rc.bottom)));
		return *pNeedRect;
	}

	bool PointInRect(int x, int y,RECT rc)
	{
		if (x<rc.right && x >rc.left && y > rc.top && y < rc.bottom) return true;
		return false;
	}

	RECT  ClientToScreenRc(HWND hWnd,RECT & rc)
	{
		POINT lt = { rc.left,rc.top }, rt = {rc.right,rc.bottom};
		ClientToScreen(hWnd, &lt);
		ClientToScreen(hWnd, &rt);
		RECT tempRect = { lt.x,lt.y,rt.x,rt.y };
		rc = tempRect;
		return tempRect;
	}

	RECT  ScreenToClientRc(HWND hWnd, RECT & rc)
	{
		POINT lt = { rc.left,rc.top }, rt = { rc.right,rc.bottom };
		ScreenToClient(hWnd, &lt);
		ScreenToClient(hWnd, &rt);
		RECT tempRect = { lt.x,lt.y,rt.x,rt.y };
		rc = tempRect;
		return tempRect;
	}
	

	string Format(char * format, ...)
	{
		va_list va;
		va_start(va, format);
		char buf[256];
		sprintf_s(buf,format,va_arg(va, char *));
		va_end(va);
		return buf;
	}

	POINT  CenterPoint(RECT rc,int offsetX,int offsetY)
	{
		POINT pt = {rc.left + (rc.right - rc.left)/2 - offsetX/2,
			rc.top + (rc.bottom - rc.top)/2 - offsetY/2 };
		return pt;
	}

	POINT LeftTopPoint(RECT rc)
	{
		POINT pt = {rc.left,rc.top};
		return pt;
	}

	char * WCharToAChar(wchar_t * Wchar)
	{
		//WideCharToMultiByte(CP_ACP,);
		return nullptr;
	}

	wchar_t * ACharToWChar(wchar_t * Wchar)
	{
		return nullptr;
	}

	MyRect::MyRect(INT left, INT top, INT right, INT bottom):mLeft(left),mRight(right),
		mTop(top),mBottom(bottom)
	{
		mRect = {mLeft,mTop,mRight,mBottom};
	}

	MyRect::MyRect(RECT desRect):mRect(desRect)
	{
	}

	RECT MyRect::Rect() const
	{
		return mRect;
	}

	MyRect::operator RECT&()
	{
		return mRect;
	}

	MyRect::operator D2D1_RECT_F&()
	{
		return RectToD2DRectF(*this);
	}
}

