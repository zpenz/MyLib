#include "stdafx.h"
#include "../MyLib/lib2d.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#pragma comment(lib,"../Debug/lib.lib")


int main()
{
	BaseWindow bs;
	LIB_CONTROL::Control ctest;
	bs.Show();
	Sleep(200);
	bs.SetWindowName("haha");
	bs.SetWindowAlpha(100);
	while (1)
	{
		//if (bs.GetHwnd() != NULL)
		//{
			//ctest.AttachParent(bs.GetHwnd());
			//if (ctest.CreateObject(500, 500,50,50)) break;
		//}
		////bitmap
		//RECT rcPicture = { 72,128,72 * 2,128 * 2 };
		//auto pBitmap = DrawManager.CreateBitmap(L"3.jpg", 72, 128);
		//DrawManager.DrawPicture(pBitmap, rcPicture);
		//RECT rcRectangle = { 0,0,72,128 };
		//DrawManager.DrawPicture(pBitmap, rcRectangle);

		////Rectangle
		//rcRectangle = { 150,150,200,200 };
		//auto pPinkBrush = DrawManager.CreateBrush(MyColor::Pink);
		//DrawManager.DrawRectangle(rcRectangle, true, pPinkBrush);
		//rcRectangle = { 400,150,450,200 };
		//DrawManager.DrawRectangle(rcRectangle, true, pPinkBrush);

		////Ellipse
		//POINT pt = { 300,300 };
		//auto pGoldBrush = DrawManager.CreateBrush(MyColor::Gold);
		//DrawManager.DrawEllipse(pt, 100.0, 50.0, false, pGoldBrush);
		//pt = { 250,300 };
		//DrawManager.DrawEllipse(pt, 10, 20, false, pGoldBrush);
		//pt = { 340,300 };
		//DrawManager.DrawEllipse(pt, 10, 20, false, pGoldBrush);
	}

	while (1)
	{

	}
    return 0;
}

