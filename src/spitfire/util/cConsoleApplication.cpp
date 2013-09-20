// Standard headers
#include <iostream>

#include <vector>
#include <list>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cConsoleApplication.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/util/thread.h>

namespace spitfire
{

  cConsoleApplication::cConsoleApplication(int argc, const char* const* argv)
  {
    spitfire::util::SetMainThread();

    spitfire::string::Init();

    // So we can run stdio and stream style i/o interchanged.
    //std::ios_base::sync_with_stdio();

    ParseArguments(argc, argv);
  }

  void cConsoleApplication::ParseArguments(int argc, const char* const* argv)
  {
    // The first argument is the executable name
    sApplicationName = spitfire::filesystem::GetFile(spitfire::string::ToString_t(argv[0]));

    // The rest of the arguments are parameters
    const size_t n = argc;
    for (size_t i = 1; i < n; i++) vArguments.push_back(spitfire::string::ToString_t(argv[i]));
  }

  void cConsoleApplication::PrintVersion() const
  {
    const string_t sVersion = _GetVersion();
    std::cout<<spitfire::string::ToUTF8(sVersion)<<std::endl;
  }

  int cConsoleApplication::Run()
  {
    bool bIsSuccessfulRun = true;
    bool bIsHandledAlready = false;

    const size_t n = GetArgumentCount();
    if (n == 0) {
      bIsHandledAlready = true;
      _PrintHelp();
    } else if (n == 1) {
      const string_t sArgument0 = GetArgument(0);
      if ((sArgument0 == TEXT("-help")) || (sArgument0 == TEXT("--help"))) {
        bIsHandledAlready = true;
        _PrintHelp();
      } else if ((sArgument0 == TEXT("-version")) || (sArgument0 == TEXT("--version"))) {
        bIsHandledAlready = true;
        PrintVersion();
      }
    }

    if (!bIsHandledAlready) bIsSuccessfulRun = _Run();

    return (bIsSuccessfulRun) ? EXIT_SUCCESS : EXIT_FAILURE;
  }
}

