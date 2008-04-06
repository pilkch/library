#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

#include <breathe/breathe.h>

#ifdef BUILD_DEBUG
#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/unittest.h>
#include <breathe/util/cSmartPtr.h>

class cSmartPtrTestClass
{
public:
	cSmartPtrTestClass(int iValue)
	{
		value = iValue;
	}

	int value;

	void FunctionA()
	{
		std::cout<<"cSmartPtrTestClass::FunctionA"<<std::endl;
	}

	void FunctionB(std::string sParameter)
	{
		std::cout<<"cSmartPtrTestClass::FunctionB sParameter="<<sParameter<<std::endl;
	}
};

class cSmartPtrUnitTest : protected breathe::util::cUnitTestBase
{
public:
	cSmartPtrUnitTest() :
		cUnitTestBase("cSmartPtrUnitTest")
	{
	}

	void Test()
	{
		breathe::util::cSmartPtr<cSmartPtrTestClass> c(new cSmartPtrTestClass(100));
		c->FunctionA();
		c->FunctionB("Chris");



		{
			//breathe::UTIL::cSmartPtr<cSmartPtrTestClass>::reference ref(c);
		}
	}
};

cSmartPtrUnitTest gSmartPtrUnitTest;
#endif //BUILD_DEBUG
