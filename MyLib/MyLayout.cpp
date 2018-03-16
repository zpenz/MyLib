#include "stdafx.h"
#include "MyLayout.h"
#include <memory>
#include <iostream>

namespace Layout
{
	using namespace Conver;

	map<wstring, fun>ControlMaping;

	std::map<int, std::wstring> LayoutParameter::ParameterMap;

	void LayoutParameter::initMap()
	{
		ParameterMap.emplace(make_pair<int, std::wstring>(1,L"text"));
		ParameterMap.emplace(make_pair<int, std::wstring>(2, L"num"));
		ParameterMap.emplace(make_pair<int, std::wstring>(3, L"text"));
		ParameterMap.emplace(make_pair<int, std::wstring>(4, L"rect"));
		ParameterMap.emplace(make_pair<int, std::wstring>(5, L"color"));
		ParameterMap.emplace(make_pair<int, std::wstring>(6, L"color"));
		ParameterMap.emplace(make_pair<int, std::wstring>(7, L"color"));
		ParameterMap.emplace(make_pair<int, std::wstring>(8, L"color"));
		ParameterMap.emplace(make_pair<int, std::wstring>(9, L"bool"));
	}

	LayoutParameter::LayoutParameter(wstring ControlType, UINT ControlID, wstring Text, RECT LayoutRect, COLORREF ForceColor, COLORREF BackColor, COLORREF HoverForceColor, COLORREF HoverBackColor, bool canDrag)
		:mControlType(ControlType), mControlID(ControlID), mText(Text), mLayoutRect(LayoutRect), mForceColor(ForceColor), mBackColor(BackColor),
		mHoverForceColor(HoverForceColor),mHoverBackColor(HoverBackColor)
	{
	}


	void LayoutParameter::pushParameter(wchar_t * element, int index)
	{
		if( element == L"RECT" || element == L"RGB")
		if (index == 1) mControlType = element;
		if (index == 2) mControlID = utoi(element);
		if (index == 3) mText = element;
		//RECT
		if (index == 5) mLayoutRect.left = utoi(element);
		if (index == 6) mLayoutRect.top = utoi(element);
		if (index == 7) mLayoutRect.right = utoi(element);
		if (index == 8) mLayoutRect.bottom = utoi(element);
		//RGB
		if (index == 10) SetR(mForceColor,utoi(element));
		if (index == 11) SetG(mForceColor,utoi(element));
		if (index == 12) SetB(mForceColor,utoi(element));
		//RGB
		if (index == 14) SetR(mBackColor, utoi(element));
		if (index == 15) SetG(mBackColor, utoi(element));
		if (index == 16) SetB(mBackColor, utoi(element));
		//RGB
		if (index == 18) SetR(mHoverForceColor, utoi(element));
		if (index == 19) SetG(mHoverForceColor, utoi(element));
		if (index == 20) SetB(mHoverForceColor, utoi(element));
		//RGB
		if (index == 22) SetR(mHoverBackColor, utoi(element));
		if (index == 23) SetG(mHoverBackColor, utoi(element));
		if (index == 24) SetB(mHoverBackColor, utoi(element));
		//drag
		if (index == 25) mCanDrag = STCAST(bool,utoi(element));
	}

	LayoutParameter MyLayout::ParseLine(wchar_t * lineBuf)
	{
		LayoutParameter tempParameter;
		int length = wstring(lineBuf).length(); 
		int index = 1;
		for(int chPos = 0;chPos<length;chPos++)
		{
			int pos;
			auto token = getNextToken(&lineBuf[chPos],pos);
			tempParameter.pushParameter(token,index);
			index++;
			chPos += pos;
		}
		return tempParameter;
	}

	wchar_t * MyLayout::getNextToken(wchar_t * startPos,int & pos)
	{
		wchar_t buf[MAX_BUF_LENGTH]; int size = 0;
		memset(buf,0,MAX_BUF_LENGTH);
		for (int chPos = 0; chPos < STCAST(int,wcslen(startPos)); chPos++)
		{
			if (startPos[chPos] == L' ' ) break;
			if (startPos[chPos] == '(' || startPos[chPos] == '(') { size++; break;}
			buf[size++] = startPos[chPos];
		}
		while (startPos[size] == L' ') size++;
		pos = size-1;

		return buf;
	}

	bool MyLayout::LoadLayoutFile(string filename, Listener * pListener)
	{
		wchar_t * pContext = nullptr;
		IS_RETURN_ERROR(!LoadFile(filename, &pContext,CP_UTF8),false,"LoadLayoutFile Failed!");
		wchar_t lineBuf[MAX_BUF_LENGTH]; int linesize = 0;
		memset(lineBuf, 0, MAX_BUF_LENGTH);

		for(int ipos = 0;ipos<wcslen(pContext);ipos++)
		{
			if(pContext[ipos] == L'\n') 
			{
				auto par = ParseLine(pContext);
				memset(lineBuf, 0, MAX_BUF_LENGTH);
				linesize = 0;
			}
			lineBuf[linesize++] = pContext[ipos];
		}
		
		return true;
	}

	MyLayout::~MyLayout()
	{
	}

	UINT FileLength(FILE * pFile)
	{
		fseek(pFile, 0, SEEK_END);
		auto size = ftell(pFile);
		rewind(pFile);
		return size;
	}

	bool LoadFile(string filename, wchar_t ** pOut, UINT codePage)
	{
		FILE * pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "r+");
		shared_ptr<void> SafeClose(NULL, [&pFile](void *) {
			if (pFile) fclose(pFile);
		});

		IS_RETURN_ERROR(!pFile, false, "fopen_s failed£¡");
		auto fileSize = FileLength(pFile);
		char * buf = new char[fileSize * sizeof(char)]();
		auto readFlag = fread(buf, sizeof(char), fileSize, pFile);
		memset(buf, 0, fileSize + 1); //fileSize + 1
		
		rewind(pFile);
		fread(buf, sizeof(char), readFlag, pFile);
		*pOut = Conver::ACharToWChar(buf);

		return true;
	}
}

