#include "soft.h"
#include <cmath>
#include <windows.h>

//DDA
bool drawLine_soft(int x0, int y0, int xEnd, int yEnd)
{
	int deltaX = xEnd - x0;
	int deltaY = yEnd - y0;
	int steep = 0;
	steep = abs(deltaY) > abs(deltaX) ? abs(deltaY) : abs(deltaX);
	int x = x0, y = y0;
	for (int i = 0; i < steep; i++)
	{
		SetPixel(GetDC(GetDesktopWindow()),x,y,RGB(255,0,0));
		x += steep / deltaX;
		y += steep / deltaY;
	}
	return true;
}