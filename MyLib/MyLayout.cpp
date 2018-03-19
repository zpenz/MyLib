#include "stdafx.h"
#include "MyLayout.h"
#include <memory>
#include <iostream>

namespace Layout
{
	using namespace Conver;

	Control * LayoutParameter::fit()
	{
		Control * pObj = nullptr;

		auto strContext = mControlType.c_str();
		auto size = mControlType.length();

		if(mControlType == wstring(L"Button"))
			pObj = MyFactory.create<Button>("Button");
		else if(mControlType == L"TitleBar")
			pObj = MyFactory.create<TitleBar>("TitleBar");
		else if (mControlType == L"MaxButton")
			pObj = MyFactory.create<MaxButton>("MaxButton");
		else if (mControlType == L"MiniButton")
			pObj = MyFactory.create<MiniButton>("MiniButton");
		else if (mControlType == L"CloseButton")
			pObj = MyFactory.create<CloseButton>("CloseButton");
		else if (mControlType == L"EditBox")
			pObj = MyFactory.create<EditBox>("EditBox");

		IS_RETURN_ERROR(!pObj,false,"fit error! reason: 控件类型有误");
		pObj->AdjustRect(mLayoutRect);
		pObj->SetForceColor(mForceColor.getRGB());
		pObj->SetBackColor(mBackColor.getRGB());
		pObj->SetHoverForceColor(mHoverForceColor.getRGB());
		pObj->SetHoverBackColor(mHoverBackColor.getRGB());
		pObj->SetText(mText);
		pObj->SetDrag(mCanDrag);
		
		return pObj;
	}


	void LayoutParameter::pushParameter(wchar_t * element, int index)
	{
		
		if (index == 1) mControlType = element;
		if (index == 2) mControlID = utoi(element);
		if (index == 3) mText = element;
		//RECT
		if (index == 5) mLayoutRect.left = utoi(element);
		if (index == 6) mLayoutRect.top = utoi(element);
		if (index == 7) mLayoutRect.right = utoi(element);
		if (index == 8) mLayoutRect.bottom = utoi(element);
		//RGB 

		if (index == 10) mForceColor.r = utoi(element);
		if (index == 11) mForceColor.g = utoi(element);
		if (index == 12) mForceColor.b = utoi(element);
		//RGB
		if (index == 14) mBackColor.r = utoi(element);
		if (index == 15) mBackColor.g = utoi(element);
		if (index == 16) mBackColor.b = utoi(element);
		//RGB
		if (index == 18) mHoverForceColor.r = utoi(element);
		if (index == 19) mHoverForceColor.g = utoi(element);
		if (index == 20) mHoverForceColor.b = utoi(element);
		//RGB
		if (index == 22) mHoverBackColor.r = utoi(element);
		if (index == 23) mHoverBackColor.g = utoi(element);
		if (index == 24) mHoverBackColor.b = utoi(element);
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
		pos = size - 1;

		return buf;
	}

	bool MyLayout::LoadLayoutFile(string filename, Listener * pListener)
	{
		wchar_t * pContext = nullptr;
		IS_RETURN_ERROR(!LoadFile(filename, &pContext,CP_UTF8),false,"LoadLayoutFile Failed!");
		wchar_t lineBuf[MAX_BUF_LENGTH]; int linesize = 0;
		memset(lineBuf, 0, MAX_BUF_LENGTH);

		for(int ipos = 0;ipos< STCAST(int,wcslen(pContext));ipos++)
		{
			if(pContext[ipos] == L'\n') 
			{
				auto par = ParseLine(lineBuf);
				Control * pControl = par.fit();
				IS_RETURN_ERROR(!pListener,false,"LoadLayoutFile Listener 为空");
				pListener->attach(pControl); //添加

				memset(lineBuf, 0, MAX_BUF_LENGTH);
				linesize = 0;
				ipos++;
			}
			
			//last row
			if(ipos == wcslen(pContext)-1)
			{
				auto par = ParseLine(lineBuf);
				Control * pControl = par.fit();
				IS_RETURN_ERROR(!pListener, false, "LoadLayoutFile Listener 为空");
				pListener->attach(pControl); //添加
				memset(lineBuf, 0, MAX_BUF_LENGTH);
				break;
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

		IS_RETURN_ERROR(!pFile, false, "fopen_s failed！");
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

