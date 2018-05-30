#include "soft.h"
#include <cmath>
#include <windows.h>

//DDA
bool drawLine_dda(int x0, int y0, int xEnd, int yEnd)
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

bool drawline_bresenham(int x0, int y0, int xEnd, int yEnd)
{
	int deltaX = abs(xEnd - x0);
	int deltaY = abs(yEnd - y0);
	int k = deltaY / deltaX;

	int dx ,dy = dx = 1;
	if (yEnd - y0 < 0) dy = -1; 
	if (xEnd - x0 < 0) dx = -1;

	//2deltaY - 2deltaX
	int deltaTwo = 2*deltaY - 2*deltaX;
	int p0 = 2 * deltaY - deltaX;
	SetPixel(GetDC(GetDesktopWindow()), x0, y0, RGB(255, 0, 0));

	int count = 0;
	int pk = p0;
	int x = x0, y = y0;
	
	if (k >= 0 && k <= 1)
	{
		while (count < deltaX)
		{
			x += dx; count++;
			if (pk > 0)
			{
				y+=dy;
				pk += deltaTwo;
			}
			else
			{
				pk += 2 * deltaY;
			}
			SetPixel(GetDC(GetDesktopWindow()), x, y, RGB(255, 0, 0));
		}
	}
	else if (k > 1)
	{
		while (count < deltaY)
		{
			y += dy; count++;
			if (pk > 0)
			{
				x+=dx;
				pk += 2*deltaX-2*deltaY;
			}
			else
			{
				pk += 2 * deltaX;
			}
			SetPixel(GetDC(GetDesktopWindow()), x, y, RGB(255, 0, 0));
		}
	}

	return true;
}

void test(void)
{
	drawline_bresenham(0, 0, 50, 100);
	drawline_bresenham(50, 100, 100, 50);
	drawline_bresenham(100, 50, 50, 25);
}
