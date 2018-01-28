#include "stdafx.h"
#include "Macros.h"

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
}

