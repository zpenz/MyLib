#include "SimpleFile.h"
#include <stdio.h>
#include <corecrt_wstdio.h>

#include "../MyLib/Macros.h"

namespace SimpleFile {
	schar * SimpleFile::ReadAllText(const char * filename)
	{
		FILE * pFile = nullptr;
		fopen_s(&pFile, filename, "r");

		IS_RETURN(!pFile, nullptr);
		fseek(pFile, 0, SEEK_END);
		int length = ftell(pFile);
		rewind(pFile);

		char buf[MAX_READ_BUFFER];
		char * const storeBuf = (char *)malloc(MAX_READ_BUFFER);
		int i = 0;
		while (ftell(pFile)<length) {
			fread(&storeBuf[i*MAX_READ_BUFFER],sizeof(buf), MAX_READ_BUFFER, pFile);
			realloc(storeBuf,sizeof(storeBuf)+MAX_READ_BUFFER);
		}

		return nullptr;
	}
	SimpleFile::SimpleFile()
	{
	}


	SimpleFile::~SimpleFile()
	{
	}

}

