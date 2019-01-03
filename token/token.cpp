
//problem:
//1. 如何防止L'ab' 赋值给一个 wchar_t
#include "pch.h"
#include <iostream>
#include <string>
#include <vector>

enum TokenKind {
	TOKEN_NUM,
	TOKEN_WORD,
	TOKEN_KEYWORD,
	TOKEN_WRITE_SPACE,
	TOKEN_EOL,
	TOKEN_UNKOWN,
	TOKEN_COMMENT
};

class Sbase {
private:
	int mILen;
	int mKind;
public:
	int Len() const { return mILen; }
	void Len(int val) { mILen = val; }
	int Kind() const { return mKind; }
	void Kind(int val) { mKind = val; }
	Sbase() :mILen(0) {}
	virtual ~Sbase() {}
};

template<class Value>
class Token : public Sbase{
private:
	Value mValue;
public:
	Value value() const { return mValue; }
	void value(Value val) { mValue = val; }
};

class Number:public Token<int>{
public:
	Number(int subject,int length) {
		Kind(TokenKind::TOKEN_NUM);
		value(subject);
		Len(length);
	}
	//isNumber 设计的容易被误用 如果传入'8L' 会被截断为'8' 
	inline static bool isNumber(const wchar_t  t) {
		return (t >= '0' && t <= '9') ? true : false;
	}
	Number & operator+(const Number& number) {
		value(value()*10+number.value());
		return *this;
	}
};

class Comment :public Token<std::wstring> {
public:
	Comment(const std::wstring& subject, int length) {
		Kind(TokenKind::TOKEN_COMMENT);
		value(subject);
		Len(length);
	}
};

class Word:public Token<std::wstring>{
public:
	 Word(const std::wstring& subject,int length) {
		Kind(TokenKind::TOKEN_WORD);
		value(subject);
		Len(length);
	}
	 //同isNumber
	inline static bool isString(wchar_t t) {
		return (t <= 'z' && t >= 'a' || t > 'A' && t < 'Z') ? true : false;
	}
};

class KeyWord :public Token<std::wstring> {
public:
	inline KeyWord(const std::wstring& subject,int length) {
		Kind(TokenKind::TOKEN_KEYWORD);
		value(subject);
		Len(length);
	}
	friend bool Word::isString(wchar_t t);
};

class Parse {
public:
	int Line() const { return iLine; }
	void Line(int val) { iLine = val; }
	int Pos() const { return iPos; }
	void Pos(int val) { iPos = val; }
	std::vector<Sbase*> TokenList() const { return mTokenList; }

	Parse() {
		mTokenList = std::vector<Sbase*>();
	}
	~Parse() {
		for (auto item : mTokenList) {
			if(item!=nullptr) delete item;
		}
	}

	void ParserString(const std::wstring &subjectStr){
		int len = subjectStr.length();
		int startPos = -1;
		for (auto index=0;index<len;index++)
		{
			if (index + 1 < len && subjectStr[index] == '/' && subjectStr[index+1] == '/') {
				//行注释
				startPos = index;
				std::wstring token = L"";
				while (index < len && subjectStr[index] != L'\n ')
				{
					token += subjectStr[index++];
				}
				mTokenList.push_back(new Comment(token, index - startPos));
			}

			if (index + 1 < len && subjectStr[index] == '/' && subjectStr[index+1] == '*') {
				//块注释
				//跳过 /*
				index += 2;
				startPos = index;
				std::wstring token = L"";
				while (index < len && index+1<len)
				{
					if (subjectStr[index] == '*' && subjectStr[index + 1] == '/') break;
					token += subjectStr[index++];
				}
				mTokenList.push_back(new Comment(token, index - startPos));
			}

			if (subjectStr[index] == ' ' || subjectStr[index] == '\n') continue;
			if (Number::isNumber(subjectStr[index])) {
				int number = 0; startPos = index;
				//认定是一个数字 读入
				std::wstring token=L"";
				while(subjectStr[index]!=' ')
				{
					auto temp = subjectStr[index];
					number = number * 10 + _wtol(&temp);
					token += subjectStr[index++];
				}
				mTokenList.push_back(new Number(number,index-startPos));
			}

			if (Word::isString(subjectStr[index])) {
				//认定是一个Word 读入
				startPos = index;
				std::wstring token = L"";
				while (subjectStr[index] != ' ')
				{
					token += subjectStr[index++];
				}
				mTokenList.push_back(new Word(token,index-startPos));
			}
		}
	}

	inline void Parser(const std::wstring &strFilePath) {

	}
private:
	int iLine;
	int iPos;
	std::vector<Sbase*> mTokenList;
};

int main()
{
	Parse* sParser = new Parse();
	std::wstring test = L"SELECT * FROM MYSQL WHERE szp = 10 haha  //I 'm comment!// \r\n hei hei I am not comment! /*sjsjs*/";
	sParser->ParserString(test);
	auto ls = sParser->TokenList();
	Sbase* pConver = nullptr;
	for(auto item : ls) {
		if (item->Kind() == TOKEN_NUM) {
			pConver = dynamic_cast<Number*>(item);
		}
		else if (item->Kind() == TOKEN_COMMENT) {
			pConver = dynamic_cast<Comment* >(item);
		}
		else
			pConver = dynamic_cast<Word*>(item);
	}
}


