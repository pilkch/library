// Spitfire headers
#include <spitfire/storage/xml.h>
#include <spitfire/util/updatechecker.h>

namespace spitfire
{
  namespace util
  {
    cUpdateChecker::cUpdateChecker(cUpdateCheckerHandler& _handler) :
      spitfire::util::cThread(soAction, "cUpdateChecker"),
      handler(_handler),
      soAction("soAction")
    {
    }

    bool cUpdateChecker::_IsToStop() const
    {
      return spitfire::util::cThread::IsToStop();
    }

    bool cUpdateChecker::IsNewerThanCurrentVersion(int iNewMajorVersion, int iNewMinorVersion) const
    {
      LOG<<"cUpdateChecker::IsVersionNewer Current version string \""<<BUILD_APPLICATION_VERSION_STRING<<"\""<<std::endl;

      const string_t sVersion = TEXT(BUILD_APPLICATION_VERSION_STRING);
      spitfire::string::cStringParser sp(sVersion);
      string_t sMajorVersion;
      if (!sp.GetToStringAndSkip(".", sMajorVersion)) {
        LOG<<"IsVersionNewer Error getting major version from version string \""<<BUILD_APPLICATION_VERSION_STRING<<"\""<<std::endl;
        return false;
      }
      const string_t sMinorVersion = sp.GetToEnd();
      const int iMajorVersion = spitfire::string::ToUnsignedInt(sMajorVersion);
      const int iMinorVersion = spitfire::string::ToUnsignedInt(sMinorVersion);

      LOG<<"cUpdateChecker::IsVersionNewer sCurrent="<<sMajorVersion<<"."<<sMinorVersion<<", current="<<iMajorVersion<<"."<<iMinorVersion<<", new="<<iNewMajorVersion<<"."<<iNewMinorVersion<<std::endl;

      const int iVersion = (10 * iMajorVersion) + iMinorVersion;
      const int iNewVersion = (10 * iNewMajorVersion) + iNewMinorVersion;
      const bool bIsVersionNewer = (iNewVersion > iVersion);
      return bIsVersionNewer;
    }

    void cUpdateChecker::_OnStatusReceived(spitfire::network::http::STATUS status)
    {
      if (status != spitfire::network::http::STATUS::ACCEPTED) {
        LOG<<"cUpdateChecker::_OnStatusReceived Bad status received "<<spitfire::network::http::GetStatusAsString(status)<<" "<<spitfire::network::http::GetStatusDescription(status)<<std::endl;
        StopThreadSoon();
      }
    }

    string_t GetContentOfChildNode(spitfire::document::cNode::const_iterator& parent, const string_t& sChildName)
    {
      // Get the child
      spitfire::document::cNode::const_iterator iterChild(parent.GetChild(sChildName));
      if (!iterChild.IsValid()) {
        LOG<<"GetContentOfChildNode Child \""<<sChildName<<"\" not found, returning \"\""<<std::endl;
        return "";
      }

      // Get the content
      spitfire::document::cNode::const_iterator iterContent(iterChild.GetFirstChild());
      if (!iterContent.IsValid()) {
        LOG<<"GetContentOfChildNode Child \""<<sChildName<<"\" content node not found, returning \"\""<<std::endl;
        return "";
      }

      return iterContent.GetContent();
    }

    void cUpdateChecker::_OnTextContentReceived(const std::string& sContent)
    {
      LOG<<"cUpdateChecker::_OnTextContentReceived"<<std::endl;

      // Read the xml document
      spitfire::xml::reader reader;
      spitfire::document::cDocument document;

      spitfire::util::cProcessInterfaceVoid interface;
      spitfire::util::PROCESS_RESULT result = reader.ReadFromString(interface, document, sContent);
      if (result != spitfire::util::PROCESS_RESULT::COMPLETE) {
        LOG<<"cUpdateChecker::_OnTextContentReceived ReadFromString failed for string \""<<sContent<<"\", returning"<<std::endl;
        return;
      }

      //<latest>
      //  <linux>
      //    <version>0.8</version>
      //    <download>http://chris.iluo.net/projects/medusa/medusa.tar.gz</download>
      //    <page>http://chris.iluo.net/projects/medusa/</page>
      //  </linux>
      //</latest>

      spitfire::document::cNode::const_iterator iterLatest(document);
      if (!iterLatest.IsValid()) return;

      iterLatest.FindChild("latest");
      if (!iterLatest.IsValid()) return;

      spitfire::document::cNode::const_iterator iterPlatform(iterLatest);
      if (!iterPlatform.IsValid()) return;

      iterPlatform.FindChild("linux");
      if (!iterPlatform.IsValid()) return;

      // Get the version
      const string_t sVersion = GetContentOfChildNode(iterPlatform, "version");
      if (sVersion.empty()) {
        LOG<<"cUpdateChecker::_OnTextContentReceived Could not find version node or content, returning"<<std::endl;
        return;
      }

      spitfire::string::cStringParser sp(sVersion);
      string_t sMajorVersion;
      sp.GetToStringAndSkip(".", sMajorVersion);
      const string_t sMinorVersion = sp.GetToEnd();
      const int iMajorVersion = spitfire::string::ToUnsignedInt(sMajorVersion);
      const int iMinorVersion = spitfire::string::ToUnsignedInt(sMinorVersion);


      // Get the page
      const string_t sDownloadPage = GetContentOfChildNode(iterPlatform, "page");
      if (sDownloadPage.empty()) {
        LOG<<"cUpdateChecker::_OnTextContentReceived Could not find page node or content, returning"<<std::endl;
        return;
      }

      // Notify the handler
      handler.OnNewVersionFound(iMajorVersion, iMinorVersion, sDownloadPage);
    }

    void cUpdateChecker::ThreadFunction()
    {
      spitfire::network::http::cHTTP connection;
      connection.Download(TEXT(BUILD_APPLICATION_WEBSITE) TEXT("version.xml"), spitfire::network::http::METHOD::GET, *this);
    }
  }
}
