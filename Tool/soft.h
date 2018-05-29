#pragma once

#ifdef SOFTAPI
#define SOFTAPI _declspec(dllimport)
#else 
#define SOFTAPI _declspec(dllexport)
#endif 

extern "C"
{
	bool SOFTAPI drawLine_soft(int x0, int y0, int xEnd, int yEnd);
}


