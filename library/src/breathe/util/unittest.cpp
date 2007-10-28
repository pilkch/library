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
		std::list<cUnitTestBase*> unittests;



		cUnitTestBase::cUnitTestBase(std::string component) :
			sComponent(component)
		{
			unittests.push_back(this);
		}

		void cUnitTestBase::Run()
		{
			success = true;

			LOG.Success("Unit Test", "Running " + sComponent);

			Test();
			
			if (true == success) LOG.Success("Unit Test", sComponent + " Successfully completed");
			else LOG.Error("Unit Test", sComponent + " FAILED");
		}
			
		void cUnitTestBase::SetFailed(std::string error)
		{
			LOG.Error("Unit Test", sComponent + " " + error);
			success = false;
		}

		void RunUnitTests()
		{
			std::list<cUnitTestBase*>::iterator iter(unittests.begin());
			std::list<cUnitTestBase*>::iterator iterEnd(unittests.end());

			while (iter != iterEnd) {
				(*iter)->Run();
			}
		}
	}
}

#endif //BUILD_DEBUG
