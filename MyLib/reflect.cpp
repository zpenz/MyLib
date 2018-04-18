#include "reflect.h"


namespace REFLECTION
{

	ReflectObject * ConstructorFactory::create(string className)
	{
		auto it = mFuncMap.find(className);
		IS_RETURN_ERROR(it == mFuncMap.end(), nullptr, (className+"���� ��û�б�ע��").c_str());
		return it->second();
	}

	bool ConstructorFactory::RegisteFunc(string className, creatFunc func)
	{
		auto pos = className.rfind(":");
		string realName = className.substr(pos + 1, className.size() - pos);
		auto chr = realName.c_str();

		IS_RETURN_ERROR(mFuncMap.find(className) != mFuncMap.end(), false, "�Ѿ����ڵ�key");
		mFuncMap.emplace(pair<string, creatFunc>(realName, func));
		return true;
	}
}

