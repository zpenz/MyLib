#pragma once
#include <string>
#include <vector>
#include <windows.h>

using namespace std;
typedef void(_stdcall * pCommandFunction)(void);

template<typename T = int>
class Point
{
public:
	T posX, posY;
	Point(T x, T y) :posX(x), posY(y) {}
	Point() :posX(0), posY(0) {};
};

class Control
{
public:
	Control();
	Control(string text,UINT ID=START_ID,int x=0,int y=0,pCommandFunction pfn=NULL);
	virtual ~Control();
	string text();
	void   setText(string text);
	UINT   GetID();
	void   SetID(UINT id);
	Point<>   GetPos();
	void      SetPos(Point<> pt);
	bool   isVisible();
	void   setVisible(bool state);
	void   setCallBack(pCommandFunction  pfn);
	void   setStyle(UINT style);
	bool   setType( string controlType);
	string getType();
protected:
	 static UINT START_ID ;
	UINT  mID;
	string mText;
	string mType;
	pCommandFunction mFunction;
	Point<> mPoint;
	UINT mStyle;
};