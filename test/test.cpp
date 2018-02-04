#include "stdafx.h"
#include "../MyLib/lib.h"
#include "../MyLib/2Draw.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{
	BaseWindow bs;
	bs.Show();
	//Sleep(500);
	//bs.SetWindowName("pSong's Window");

	//RECT rcPicture = { 500,400,600,450 };
	//DrawManager.DrawRectWithText(rcPicture,"haha",MyColor::Blue,MyColor::Black,1);
	//auto testH = FindWindowA("Notepad","新建文本文档.txt - 记事本");
	
	while (1)
	{
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

    return 0;
}

