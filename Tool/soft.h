#pragma once

#ifdef SOFTAPI
#define SOFTAPI _declspec(dllimport)
#else 
#define SOFTAPI _declspec(dllexport)
#endif 

extern "C"
{
	void SOFTAPI test(void);

	bool SOFTAPI drawLine_dda(int x0, int y0, int xEnd, int yEnd);

	bool SOFTAPI drawline_bresenham(int x0, int y0, int xEnd, int yEnd);
}


