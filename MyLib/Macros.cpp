#include "stdafx.h"
#include "Macros.h"

void ErrorMessage(const char * _error)
{
	MessageBox(NULL, _error, "error", MB_OK);
	exit(0);
}

namespace Conver
{
	string Format(char * format, ...)
	{
		va_list va;
		va_start(va, format);
		char buf[256];
		sprintf_s(buf,format,va_arg(va, char *));
		va_end(va);
		return buf;
	}

	POINT  CenterPoint(RECT rc,float offsetX,float offsetY)
	{
		POINT pt = {rc.left + (rc.right - rc.left)*0.5 - offsetX*0.5,
			rc.top + (rc.bottom - rc.top)*0.5 - offsetY*0.5 };
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
}

