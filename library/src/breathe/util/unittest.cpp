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

namespace breathe
{
	namespace util
	{
		std::list<cUnitTestBase*>* unittests = nullptr;


		cUnitTestBase::cUnitTestBase(std::string component) :
			sComponent(component)
		{
			if (nullptr == unittests) unittests = new std::list<cUnitTestBase*>;

			unittests->push_back(this);
		}
		
		cUnitTestBase::~cUnitTestBase()
		{
			unittests->remove(this);
			if (unittests->empty()) SAFE_DELETE(unittests);
		}

		void cUnitTestBase::Run()
		{
			success = true;

			CONSOLE.Success("Unit Test", "Running " + sComponent);

			Test();
			
			if (true == success) CONSOLE.Success("Unit Test", sComponent + " Successfully completed");
			else CONSOLE.Error("Unit Test", sComponent + " FAILED");
		}
			
		void cUnitTestBase::SetFailed(std::string error)
		{
			CONSOLE.Error("Unit Test", sComponent + " " + error);
			success = false;
		}

		void RunUnitTests()
		{
			std::list<cUnitTestBase*>::iterator iter(unittests->begin());
			std::list<cUnitTestBase*>::iterator iterEnd(unittests->end());

			while (iter != iterEnd) {
				(*iter)->Run();
				iter++;
			}
		}
	}
}

#endif //BUILD_DEBUG
