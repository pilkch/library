#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

// Spitfire headers
#include <spitfire/communication/http.h>
#include <spitfire/util/signalobject.h>
#include <spitfire/util/thread.h>

namespace spitfire
{
  namespace util
  {
    class cUpdateCheckerHandler
    {
    public:
      virtual ~cUpdateCheckerHandler() {}

      virtual void OnNewVersionFound(int iMajorVersion, int iMinorVersion, const string_t& sDownloadPage) = 0;
    };

    class cUpdateChecker : public spitfire::util::cThread, public spitfire::network::http::cRequestListener
    {
    public:
      explicit cUpdateChecker(cUpdateCheckerHandler& handler);

      bool IsNewerThanCurrentVersion(int iNewMajorVersion, int iNewMinorVersion) const;

    private:
      // For the request listener
      virtual bool _IsToStop() const override;
      virtual void _OnStatusReceived(spitfire::network::http::STATUS status) override;
      virtual void _OnTextContentReceived(const std::string& sContent) override;

      virtual void ThreadFunction() override;

      cUpdateCheckerHandler& handler;

      spitfire::util::cSignalObject soAction;
    };
  }
}

#endif // UPDATECHECKER_H
