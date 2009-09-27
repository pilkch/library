#include <cassert>

#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <list>
#include <map>
#include <iterator>
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/cVar.h>

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
      CONSOLE<<breathe::string::ToUTF8(iter->first) + " \"" + breathe::string::ToUTF8((iter->second)->GetString()) + "\""<<std::endl;
      iter++;
    };
  }

#ifdef BUILD_DEBUG
  class cVarUnitTest : protected util::cUnitTestBase
  {
  public:
    cVarUnitTest() :
      cUnitTestBase(TEXT("cVarUnitTest"))
    {
    }

    void Test()
    {
      var::VarSet(TEXT("bool"), true);
      var::VarSet(TEXT("int"), 180);
      var::VarSet(TEXT("float"), 123.45f);
      var::VarSet(TEXT("string"), TEXT("this is a string"));

      TestVariable(TEXT("bool"));
      TestVariable(TEXT("int"));
      TestVariable(TEXT("float"));
      TestVariable(TEXT("string"));
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
