// Standard libraries
#include <cassert>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <map>
#include <sstream>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/thread.h>

#include <breathe/math/math.h>

#include <breathe/storage/xml.h>

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>
#include <breathe/communication/http.h>
#include <breathe/communication/feed.h>

namespace breathe
{
  namespace network
  {
    void cFeed::ParseAtomFeed(const std::string& sContent)
    {
      LOG<<"cFeed::ParseAtomFeed"<<std::endl;

      Clear();

      xml::cNode root;

      xml::cReader reader;
      if (!reader.ReadFromString(root, sContent)) {
        LOG<<"cFeed::ParseAtomFeed XML Could not be read from string, returning"<<std::endl;
        return;
      }

      // Now load all the rest from the config file
      xml::cNode::iterator iter(root);

      if (!iter.IsValid()) {
        LOG<<"cFeed::ParseAtomFeed XML document not found, returning"<<std::endl;
        return;
      }

      iter.FindChild("feed");
      if (!iter.IsValid()) {
        LOG<<"cFeed::ParseAtomFeed feed node not found, returning"<<std::endl;
        return;
      }

      std::string sXMLNameSpace;
      if (iter.GetAttribute("xmlns", sXMLNameSpace)) {
        LOG<<"cFeed::ParseAtomFeed xmlns found \""<<sXMLNameSpace<<"\""<<std::endl;
        ASSERT(sXMLNameSpace == "http://www.w3.org/2005/Atom");
      }

      xml::cNode::iterator iterTitle(iter.GetChild("title"));
      if (iterTitle.IsValid()) {
        title = iterTitle.GetChildContent();
        LOG<<"cFeed::ParseAtomFeed title found \""<<title<<"\""<<std::endl;
      }

      iter.FindChild("entry");
      if (!iter.IsValid()) {
        LOG<<"cFeed::ParseAtomFeed entry node not found, returning"<<std::endl;
        return;
      }

      while (iter.IsValid()) {
        cFeedArticle* pArticle = new cFeedArticle;

        xml::cNode::iterator iterId(iter.GetChild("id"));
        if (iterId.IsValid()) {
          pArticle->id = iterId.GetChildContent();
          //LOG<<"cFeed::ParseAtomFeed id found \""<<pArticle->id<<"\""<<std::endl;
        }

        xml::cNode::iterator iterTitle(iter.GetChild("title"));
        if (iterTitle.IsValid()) {
          pArticle->title = iterTitle.GetChildContent();
          //LOG<<"cFeed::ParseAtomFeed title found \""<<pArticle->title<<"\""<<std::endl;
        }

        xml::cNode::iterator iterDate(iter.GetChild("updated"));
        if (iterDate.IsValid()) {
          pArticle->date = iterDate.GetChildContent();
          //LOG<<"cFeed::ParseAtomFeed id found \""<<pArticle->date<<"\""<<std::endl;
        }

        xml::cNode::iterator iterSummary(iter.GetChild("summary"));
        if (iterSummary.IsValid()) {
          pArticle->summary = iterSummary.GetChildContent();
          //LOG<<"cFeed::ParseAtomFeed summary found \""<<pArticle->summary<<"\""<<std::endl;
        }

        xml::cNode::iterator iterLink(iter.GetChild("link"));
        if (iterLink.IsValid()) {
          iterLink.GetAttribute("rel", pArticle->link.rel);
          iterLink.GetAttribute("type", pArticle->link.type);
          iterLink.GetAttribute("href", pArticle->link.href);
          //LOG<<"cFeed::ParseAtomFeed link found \""<<pArticle->link.rel<<"\" \""<<pArticle->link.type<<"\" \""<<pArticle->link.href<<"\""<<std::endl;
        }

        xml::cNode::iterator iterContent(iter.GetChild("content"));
        if (iterContent.IsValid()) {
          iterContent.GetAttribute("type", pArticle->content.type);
          iterContent.GetAttribute("src", pArticle->content.src);
          //LOG<<"cFeed::ParseAtomFeed content found \""<<pArticle->content.type<<"\" \""<<pArticle->content.src<<"\""<<std::endl;
        }

        articles.push_back(pArticle);

        iter.Next("entry");
      };

      LOG<<"cFeed::ParseAtomFeed returning"<<std::endl;
    }

    void cFeed::DownloadFeed(const string_t& sURL)
    {
      /*Clear();

      cHTTPDownloader downloader;

      downloader.Download(sURL);

      while (downloader.IsConnected()) {
        Yield();
      };

      if (!downloader.IsSuccessfulDownload()) return;

      ParseAtomFeed(downloader.GetContent());*/
    }
  }
}
