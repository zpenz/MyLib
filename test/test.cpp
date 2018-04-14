#include "stdafx.h"
#include "../MyLib/lib.h"
#include "../MyLib/2Draw.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#pragma comment(lib,"../Debug/lib.lib")
#include <iostream>
using namespace std;

int main()
{
	BaseWindow bs;
	bs.Show();
	
	while (1)
	{
		POINT  * pPoint=new POINT();
		GetCursorPos(pPoint);
		COLORREF color = GetPixel(GetDC(NULL), pPoint->x,pPoint->y);
		cout <<(UINT)GetRValue(color)<<(UINT)GetGValue(color)<< (UINT)GetBValue(color)<<endl;
		Sleep(1000);

		//SAVE
		bs.mListener.AddClickFuncByID(120, [&]() {
			printf("开始保存...\n");
			Layout::ControlLayout.SaveLayoutFile("D:\\window.layout", &bs.mListener);
			printf("保存完毕...\n");
		});

		//LabelBox
		bs.mListener.AddClickFuncByID(126, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"LabelBox";
			printf("选择...\n");
		});
	}

    return 0;
}

