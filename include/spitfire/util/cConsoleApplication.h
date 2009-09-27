#ifndef CCONSOLEAPPLICATION_H
#define CCONSOLEAPPLICATION_H

#include <spitfire/util/cString.h>

namespace spitfire
{
  class cConsoleApplication
  {
  public:
    cConsoleApplication(int argc, const char* const* argv);
    virtual ~cConsoleApplication() {}

    const string_t& GetApplicationName() const { return sApplicationName; }

    size_t GetArgumentCount() const { return vArguments.size(); }
    const string_t& GetArgument(size_t index) const { ASSERT(index < vArguments.size()); return vArguments[index]; }

    int Run();

  private:
    void ParseArguments(int argc, const char* const* argv);
    void PrintVersion() const;

    virtual void _PrintHelp() const = 0;
    virtual string_t _GetVersion() const = 0;
    virtual bool _Run() = 0; // NOTE: This may not be run at all, for example if "--help" is the first argument

    string_t sApplicationName;
    std::vector<string_t> vArguments;
  };
}

#endif // CCONSOLEAPPLICATION_H
