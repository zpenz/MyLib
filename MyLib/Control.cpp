#include "Control.h"
#include <algorithm>

UINT Control::START_ID = 678 + 1;

class MyString :public std::string 
{
public:
	string ms;
	MyString(string s) :ms(s) {}
	bool toUpper()
	{
		if (empty()) return false;
		auto cString = c_str();
		char c;
		do 
		{
			static int i = 0;
			c = c_str()[i++];
			if ((int)c > 97) c = c - 32;
		} while (!c);
		return true;
	}
	operator string() { return ms; }
};

vector<string> typeV = {"EDIT","BUTTON","STATIC","LISTBOX","SCROLLBAE","COMBOBOX",
"MDICLIENT","RichEdit","RICHEDIT_CLASS"};

Control::Control()
{
	Control("", START_ID++);
}

Control::Control(string text, UINT ID, int x, int y, pCommandFunction pfn) :mText(text), mID(ID),
mPoint(x, y), mFunction(pfn),mStyle(WS_CHILD|WS_VISIBLE),mType("BUTTON")
{
}

Control::~Control()
{
}

string Control::text()
{
	return mText;
}

void Control::setText(string text)
{
	if(!text.empty()) mText = text;
}

UINT Control::GetID()
{
	return mID;
}

void Control::SetID(UINT id)
{
	mID = id;
}

Point<> Control::GetPos()
{
	return mPoint;
}

void Control::SetPos(Point<> pt)
{
	mPoint = pt;
}

bool Control::isVisible()
{
	return static_cast<bool>(mStyle & WS_VISIBLE);
}

void Control::setVisible(bool state)
{
	mStyle =  state ? (mStyle |WS_VISIBLE ): (mStyle|~WS_VISIBLE);
}

void Control::setCallBack(pCommandFunction  pfn)
{
	mFunction = pfn;
}

void Control::setStyle(UINT style)
{
	mStyle = style;
}

bool Control::setType( string controlType)
{//test ... not 
	auto tempType = (string)strupr(const_cast<char *>(controlType.c_str()));
	MyString s(controlType);
	s.toUpper();
	tempType = s.c_str();
	if (find_if(typeV.begin(),typeV.end(), [&tempType](vector<string>::reference & r)->bool{
		return r == tempType;
	}) == typeV.end())
		return false;
	mType = controlType;
	return true;
}

string Control::getType()
{
	return mType;
}
