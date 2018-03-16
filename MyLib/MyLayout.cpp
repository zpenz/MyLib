#include "stdafx.h"
#include "MyLayout.h"
#include <memory>

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
		if (index == 1) mControlType = element;
		if (index == 2) mControlID = utoi(element);
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
		if (index == 1) mControlType = element;
	}

	LayoutParameter MyLayout::ParseLine(wchar_t * lineBuf)
	{
		LayoutParameter tempParameter;
		int length = wstring(lineBuf).length(); 
		for(int chPos = 0;chPos<length;chPos++)
		{
			int pos;
			auto token = getNextToken(&lineBuf[chPos],pos);
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
		ParseLine(pContext);
		return false;
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

