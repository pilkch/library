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
  std::map<string_t, cVar*> var::mVar;


  cVar* var::VarFind(const string_t& name)
  {
    std::map<string_t, cVar*>::iterator iter = mVar.begin();
    const std::map<string_t, cVar*>::iterator iterEnd = mVar.end();

    while (iter != iterEnd) {
      if (name == iter->first) return iter->second;

      iter++;
    };

    return NULL;
  }

  void var::PrintAll()
  {
    std::map<string_t, cVar*>::iterator iter = mVar.begin();
    const std::map<string_t, cVar*>::iterator iterEnd = mVar.end();

    while (iter != iterEnd) {
      CONSOLE<<iter->first + " \"" + (iter->second)->GetString() + "\""<<std::endl;
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
    void TestVariable(const string_t& variable)
    {
      breathe::cVar* p = var::VarFind(variable);
      assert(p != NULL);

      bool b = p->GetBool();
      int i = p->GetInt();
      float f = p->GetFloat();
      string_t s = p->GetString();

      CONSOLE<<variable<<"=(\""<<s<<"\","<<b<<","<<i<<","<<f<<")"<<std::endl;
    }
  };

  cVarUnitTest gVarUnitTest;
#endif
}
