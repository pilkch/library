#if defined (_MSC_VER) && _MSC_VER <= 1200  // MSVC++ 6.0
#  pragma warning(disable: 4786)  
#endif

#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <list>
#include <map>
#include <iterator>
#include <vector>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cVar.h>
#include <breathe/util/unittest.h>

namespace breathe
{
	std::map<std::string, cVar<std::string>*> var::mVar;


	cVar<std::string>* var::VarFind(const std::string& name)
	{
		std::map<std::string, cVar<std::string>*>::iterator iter=mVar.begin();

		while(iter!=mVar.end())
		{
			if (name == iter->first)
				return iter->second;

			iter++;
		};

		return NULL;
	}

	void var::PrintAll()
	{
		std::map<std::string, cVar<std::string>* >::iterator iter=mVar.begin();

		while(iter!=mVar.end())
		{
			CONSOLE<<iter->first + " \"" + (iter->second)->GetString() + "\"";
			iter++;
		};
	}

#ifdef BUILD_DEBUG
	class cVarUnitTest : protected util::cUnitTestBase
	{
	public:
		cVarUnitTest() :
			cUnitTestBase("cVarUnitTest")
		{
		}

		void Test()
		{
			var::VarSet("bool", true);
			var::VarSet("int", 180);
			var::VarSet("float", 123.45f);
			var::VarSet("string", "this is a string");

			TestVariable("bool");
			TestVariable("int");
			TestVariable("float");
			TestVariable("string");
		}

	private:
		void TestVariable(const std::string& variable)
		{
			breathe::cVar<std::string>* p = var::VarFind(variable);
			assert(p != NULL);

			std::string s = p->GetString();
			bool b = p->GetBool();
			unsigned int ui = p->GetUnsignedInt();
			int i = p->GetInt();
			float f = p->GetFloat();

			CONSOLE<<variable<<"=(\""<<s<<"\","<<b<<","<<ui<<","<<i<<","<<f<<")"<<std::endl;
		}
	};

	cVarUnitTest gVarUnitTest;
#endif
}
