#include "stdafx.h"
#include "MyLayout.h"
#include <memory>
#include <iostream>

namespace Layout
{
	using namespace Conver;

	LayoutParameter::LayoutParameter(wstring ControlType, UINT ID, wstring text, RECT layoutRect, ColorStruct fcolor, ColorStruct bcolor, ColorStruct hfcolor, ColorStruct hbcolor,
		bool candrag, bool haveBoard, ColorStruct boardColor, float boardSize):
		mControlType(ControlType),mControlID(ID),mText(text),mLayoutRect(layoutRect),mForceColor(fcolor),mBackColor(bcolor),
		mHoverForceColor(hfcolor),mHoverBackColor(hbcolor),mCanDrag(candrag),mHaveBoard(haveBoard),mBoardColor(boardColor),mBoardSize(boardSize)
	{
	}

	Control * LayoutParameter::fit()
	{
		Control * pObj = nullptr;

		// 直接使用Control基类指针 将ReflectObject指针转换为Control(dynamic_cast) 
		pObj = MyFactory.create<Control>(WCharToAChar(COCAST(wchar_t *,mControlType.c_str())));
		IS_RETURN_ERROR(!pObj,nullptr,"fit error! reason: 控件类型有误");
		if (mText == L"NULL") mText = L""; //NULL 

		pObj->SetClassName(COCAST(wchar_t *,mControlType.c_str()));
		pObj->SetID(mControlID);
		pObj->AdjustRect(mLayoutRect);
		pObj->SetForceColor(mForceColor);
		pObj->SetBackColor(mBackColor);
		pObj->SetHoverForceColor(mHoverForceColor);
		pObj->SetHoverBackColor(mHoverBackColor);
		pObj->SetText(mText);
		pObj->SetDrag(mCanDrag);
		pObj->SetBoard(mHaveBoard);
		pObj->setBoardColor(mBoardColor);
		pObj->setBoardSize(mBoardSize);
		return pObj;
	}

	vector<wstring> MyLayout::sSkipTokens =
	{
		L"RECT",L"RGB",L"Rect"
	};
	
	LayoutParameter MyLayout::ParseLine(wchar_t * lineBuf)
	{
		vector<wchar_t *> vParams;
		int length = wstring(lineBuf).length();
		for (int chPos = 0; chPos < length; chPos++)
		{
			int pos;
			auto token = getNextToken(&lineBuf[chPos], pos);

			if (find_if(sSkipTokens.begin(), sSkipTokens.end(), [&token](wstring skipToken)
			{
				if (token == skipToken) return true;
				return false;
			}) != sSkipTokens.end())
			{
				chPos += pos;
				SAFE_DELETE(token);
				continue;
			}

			vParams.push_back(token);
			chPos += pos;
		}

		shared_ptr<void> SafeDelete(NULL, [&vParams](void *)
		{
			for_each(vParams.begin(), vParams.end(), [](wchar_t * pBuf) {
				SAFE_DELETE(pBuf);
			});
		});

		return LayoutParameter(
			vParams[0], utoi(vParams[1]), vParams[2],
			MyRect(utoi(vParams[3]), utoi(vParams[4]), utoi(vParams[5]), utoi(vParams[6])),
			ColorStruct(utoi(vParams[7]), utoi(vParams[8]), utoi(vParams[9])),
			ColorStruct(utoi(vParams[10]), utoi(vParams[11]), utoi(vParams[12])),
			ColorStruct(utoi(vParams[13]), utoi(vParams[14]), utoi(vParams[15])),
			ColorStruct(utoi(vParams[16]), utoi(vParams[17]), utoi(vParams[18])),
			utob(vParams[19]),
			utob(vParams[20]),
			ColorStruct(utoi(vParams[21]), utoi(vParams[22]), utoi(vParams[23])),
			utof(vParams[24]));
	}

	wchar_t * MyLayout::getNextToken(wchar_t * startPos,int & pos)
	{
		wchar_t buf[MAX_BUF_LENGTH]; int size = 0;
		memset(buf,0,MAX_BUF_LENGTH);
		for (int chPos = 0; chPos < STCAST(int,wcslen(startPos)); chPos++)
		{
			if (startPos[chPos] == L' ' ) break;
			if (startPos[chPos] == '(' || startPos[chPos] == '('|| startPos[chPos] == ')') { size++; break;}
			buf[size++] = startPos[chPos];
		}
		wchar_t * ret = new wchar_t[size+1]();
		wmemcpy(ret, buf, size);
		ret[size] = '\0';
		while (startPos[size] == L' ') size++;
		pos = size - 1;

		return ret;
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
				IS_RETURN_ERROR(!pListener,false,"LoadLayoutFile Listener null");
				if(pControl) pListener->attach(pControl); //添加
				memset(lineBuf, 0, MAX_BUF_LENGTH);
				linesize = 0;
				ipos++;
			}
			
			//last row
			if(ipos == wcslen(pContext)-1)
			{
				auto par = ParseLine(lineBuf);
				Control * pControl = par.fit();
				IS_RETURN_ERROR(!pListener, false, "LoadLayoutFile Listener null");
				IS_RETURN_ERROR(!pControl,false,"LoadLayoutFile pControl null")
				if (pControl) pListener->attach(pControl); //添加
				memset(lineBuf, 0, MAX_BUF_LENGTH);
				break;
			}
			lineBuf[linesize++] = pContext[ipos];
		}
		return true;
	}

	bool MyLayout::SaveLayoutFile(string outFileName, Listener * pListener)
	{
		IS_RETURN_ERROR(outFileName.empty() || !pListener, false, "输出的layout文件名或者Listener为空");
		FILE * pfile = nullptr;
		_wfopen_s(&pfile, ACharToWChar(COCAST(char *,outFileName.c_str())), L"w+");
		IS_RETURN_ERROR(!pfile,false,"file open error");

		shared_ptr<void> pExit(NULL, [&](void*) {
			if (pfile) fclose(pfile);
		});

		auto ListenerList = pListener->Obj();
		for_each(ListenerList.begin(), ListenerList.end(), [&](Control * pControl)
		{
			auto name = WCharToAChar(COCAST(wchar_t *, pControl->getClassName().c_str()));
			fprintf_s(pfile, "%s ", name);
			fprintf_s(pfile, "%d ", pControl->getID());
			auto tempText = pControl->Text();
			if (tempText.empty()) fprintf_s(pfile, "NULL");
			auto text = COCAST(wchar_t *, tempText.c_str());
			_wsetlocale(0, L"chs");
			fprintf_s(pfile, "%ls ", text);
			auto tempRect = pControl->getRect();
			fprintf_s(pfile, "RECT(%d %d %d %d) ", tempRect.left, tempRect.top, tempRect.right, tempRect.bottom);
			auto tempColor = pControl->ForceColor();
			fprintf_s(pfile, "RGB(%d %d %d) ", GetRValue(tempColor), GetGValue(tempColor), GetBValue(tempColor));
			tempColor = pControl->BackColor();
			fprintf_s(pfile, "RGB(%d %d %d) ", GetRValue(tempColor), GetGValue(tempColor), GetBValue(tempColor));
			tempColor = pControl->HoverForceColor();
			fprintf_s(pfile, "RGB(%d %d %d) ", GetRValue(tempColor), GetGValue(tempColor), GetBValue(tempColor));
			tempColor = pControl->HoverBackColor();
			fprintf_s(pfile, "RGB(%d %d %d) ", GetRValue(tempColor), GetGValue(tempColor), GetBValue(tempColor));
			fprintf_s(pfile, "%d ", pControl->CanDrag()?1:0);
			fprintf_s(pfile, "%d ", pControl->HaveBoard()?1:0);
			tempColor = pControl->getBoardColor();
			fprintf_s(pfile, "RGB(%d %d %d) ", GetRValue(tempColor), GetGValue(tempColor), GetBValue(tempColor));
			fprintf_s(pfile, "%f\n", pControl->getBoardSize());

		});
		return true;
	}

	bool MyLayout::WriteDataWithChar(FILE * pFile, void * data, wchar_t append)
	{
		IS_RETURN_ERROR(pFile,false,"pFile为空");
		fwrite(data, sizeof(data), 1, pFile);
		fwrite(&append, sizeof(append), 1, pFile);
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
		*pOut = Conver::ACharToWChar(buf,CP_ACP);

		return true;
	}
}

