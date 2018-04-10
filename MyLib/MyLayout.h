#pragma once
#include "Control.h"
#include <map>
#include <typeinfo>

namespace Layout
{
	using namespace std;
	using namespace LIB_CONTROL;

	UINT FileLength(FILE * pFile);

	bool LoadFile(string filename, wchar_t ** pOut,UINT codePage = CP_ACP);

	class LayoutParameter
	{
	public:

		LayoutParameter() {}

		bool     mCanDrag;

		wstring mControlType;

		UINT mControlID;

		wstring mText;

		RECT  mLayoutRect;

		ColorStruct mForceColor;

		ColorStruct mBackColor;

		ColorStruct mHoverForceColor;

		ColorStruct mHoverBackColor;

		Control * fit(); 

		void pushParameter(wchar_t * element,int index);
	};

	class MyLayout
	{
	public:

		SINGLE_INSTANCE(MyLayout)

		LayoutParameter ParseLine(wchar_t * lineBuf);

		wchar_t * getNextToken(wchar_t * startPos, int & pos);

		bool LoadLayoutFile(string filename, Listener * pListener);

		bool SaveLayoutFile(string outFileName,Listener * pListener);

		bool WriteDataWithChar(FILE * pFile, void * data, wchar_t append = L' ');

		~MyLayout();
	};
#define ControlLayout MyLayout::getInstance()

}



