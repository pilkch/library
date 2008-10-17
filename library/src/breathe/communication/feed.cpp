// Standard libraries
#include <cassert>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

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

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>
#include <breathe/communication/http.h>
#include <breathe/communication/feed.h>

namespace breathe
{
  namespace network
  {
    void cFeedDownloader::ParseAtomFeed(const std::string& sContent)
    {
      make sure that cXML::cXML() takes const string_t& or (const std::string& and const std::wstring&)

      in namespace xml:
      typedef cNode cXML;
      cXML xml(sContent);

      should change constructor to take no parameters
      then we call:
      CreateFromUTF8(const std::string& sBuffer);
      CreateFromString(const string_t& sBuffer);
      CreateFromFile(const string_t& sFilename);

      // Now load all the rest from the config file
      xml::cXML root;
      root.CreateFromUTF8(content);
      if (!root.IsValid()) return;

      xml::cNode::iterator iter(root);
      if (!iter.IsValid()) return;

      iter.FindChild("feed");
      if (!iter.IsValid()) return;

      iter.FindChild("entry");
      if (!iter.IsValid()) return;

      while (iter.IsValid()) {
          breathe::xml::cNode::iterator entry(iter);

          cFeedArticle* pArticle = new cFeedArticle;

          iter.FirstChild();
          while (iter.IsValid()) {
            if (iter.GetName() == "title") pArticle->title = iter.GetContent();
            else if (iter.GetName() == "link") iter.GetAttribute("href", pArticle->url);
            else if (iter.GetName() == "id") pArticle->uniqueid = iter.GetContent();
            else if (iter.GetName() == "updated") pArticle->date = iter.GetContent();
            else if (iter.GetName() == "summary") pArticle->content = iter.GetContent();

            iter.Next();
          };

          articles.push_back(pArticle);

          iter.Next("entry");
      };
    }

    void cFeedDownloader::DownloadFeed(const string_t& sURL)
    {
      Clear();

      cHTTPDownloader downloader;

      downloader.Download(sURL);

      while (downloader.IsConnected()) {
          yield;
      };

      if (!downloader.IsSuccessfulDownload()) return;

      ParseAtomFeed(downloader.GetContent());
    }
  }
}
