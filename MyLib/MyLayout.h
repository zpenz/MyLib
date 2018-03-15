#pragma once
#include "Control.h"
#include <map>

namespace Layout
{
	using namespace std;
	using namespace LIB_CONTROL;

	typedef struct  
	{
		wstring ControlType;
		UINT ControlID;
		wstring Text;
		RECT  LayoutRect;
		COLORREF ForceColor;
		COLORREF BackColor;
		COLORREF HoverForceColor;
		COLORREF HoverBackColor;
		bool     canDrag;
	}LayoutParameter;




	typedef Control *(*fun)();



	class MyLayout
	{
	public:

		static map<wstring, fun> ControlMaping;

		SINGLE_INSTANCE(MyLayout)

		bool LoadLayoutFile(wstring filename,Listener * pListener);

		~MyLayout();
	};


}



