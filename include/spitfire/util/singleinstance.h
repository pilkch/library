#ifndef UTIL_SINGLEINSTANCE_H
#define UTIL_SINGLEINSTANCE_H

// Spitfire headers
#include <spitfire/util/string.h>

namespace spitfire
{
  namespace util
  {
    namespace singleinstance
    {
      class cClient;
      class cServer;

      class cSingleInstance
      {
      public:
        cSingleInstance();
        ~cSingleInstance();

        bool StartServer();
        void StopServerSoon();
        void StopServerNow();

        bool StartClient();
        bool SendToServer(const string_t& sText);

      private:
        cServer* pServer;
        cClient* pClient;
      };
    }
  }
}

#endif // UTIL_SINGLEINSTANCE_H
