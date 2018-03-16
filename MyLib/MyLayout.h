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
		void initMap();
	public:

		LayoutParameter() {}

		wstring mControlType;
		UINT mControlID;
		wstring mText;
		RECT  mLayoutRect;
		COLORREF mForceColor;
		COLORREF mBackColor;
		COLORREF mHoverForceColor;
		COLORREF mHoverBackColor;
		bool     mCanDrag;

		LayoutParameter(wstring ControlType,
		UINT ControlID,
		wstring Text,
		RECT  LayoutRect,
		COLORREF ForceColor,
		COLORREF BackColor,
		COLORREF HoverForceColor,
		COLORREF HoverBackColor,
		bool     canDrag);

		void pushParameter(wchar_t * element,int index);

		static map<int, wstring> ParameterMap;
	};

	
	typedef Control*(*fun)();


	template<typename T>
	class Construcor 
	{
	public:
		static map<wstring, fun> mControlMaping;

		T * createObject(LayoutParameter lp);

		Construcor() {}
	};

	template<typename T>
	inline T * Construcor<T>::createObject(LayoutParameter lp)
	{
		auto classname = typeid(T).name();
		return new T(lp.mText,lp.mLayoutRect,lp.mForceColor,lp.mBackColor,lp.mHoverForceColor,lp.mHoverBackColor);
	}


	class MyLayout
	{
	public:

		SINGLE_INSTANCE(MyLayout)

		LayoutParameter ParseLine(wchar_t * lineBuf);

		wchar_t * getNextToken(wchar_t * startPos, int & pos);

		bool LoadLayoutFile(string filename, Listener * pListener);

		~MyLayout();
	};

}



