#pragma once
#include "../MyLib/Macros.h"

#ifdef UNICODE
#define schar wchar_t
#else
#define schar char
#endif

#ifdef SIMPLEFILEAPI 
#define SIMPLEFILEAPI _declspec(dllimport)
#else
#define SIMPLEFILEAPI _declspec(dllexport)
#endif // SIMPLEFILEAPI 


namespace SimpleFile {

#define MAX_READ_BUFFER 0x1000
	
	extern "C"
	{
		class SIMPLEFILEAPI SimpleFile
		{
		public:

			schar * ReadAllText(const char * filename);

			SimpleFile();

			~SimpleFile();
		};

		
	}

}


