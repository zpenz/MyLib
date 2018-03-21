#pragma once
#include "Macros.h"
//ºÚµ•∑¥…‰ 
namespace REFLECTION
{

	using namespace std;

	class ReflectObject
	{
	public:
		virtual ~ReflectObject()
		{

		}
	};

	typedef ReflectObject*(*creatFunc)();

	class ConstructorFactory
	{
		SINGLE_INSTANCE(ConstructorFactory)
			//class-name -------- creator
			map<string, creatFunc> mFuncMap;

	public:

		ReflectObject * create(string className);

		bool RegisteFunc(string className, creatFunc func);

		template<typename T>
		T * create(string className)
		{
			auto * obj = create(className);
			IS_RETURN_ERROR(!obj, nullptr, "null object...");
			T * reallyObj = dynamic_cast<T*>(obj);
			IS_RETURN_ERROR(!obj, nullptr, "null realobj...");
			return reallyObj;
		}
	};

#define MyFactory ConstructorFactory::getInstance()

	template<typename T>
	class REFLECT :public ReflectObject
	{
	public:
		static ReflectObject * createObject()
		{
			return DYCAST(ReflectObject *, new T());
		}

		struct reg
		{
			reg()
			{
				MyFactory.RegisteFunc(typeid(T).name(), createObject);
			}

			inline void nothing() {}
		};

		static reg mReg;

		REFLECT()
		{
			mReg.nothing();
		}

		virtual ~REFLECT()
		{

		}
	};

	template<typename T>
	typename REFLECT<T>::reg REFLECT<T>::mReg;
}
