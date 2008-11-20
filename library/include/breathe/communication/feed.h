#ifndef CFEED_H
#define CFEED_H

#include <breathe/util/datetime.h>

namespace breathe
{
  namespace network
  {
    // This is a feed reader for reading Atom feeds
    // We do not read RSS feeds or even complex encoding in Atom feeds
    // It is basically the bare minimum required to read an update atom feed that we have authored ourselves

    class cFeedArticle
    {
    public:
      friend class cFeedDownloader;

      const util::cDateTime& GetDate() const { return date; }
      const std::string& GetTitle() const { return title; }
      const std::string& GetContent() const { return content; }

    private:
      NO_COPY(cFeedArticle);

      std::string title;
      std::string url;
      std::string uniqueid;
      util::cDateTime date;
      std::string content;
    };

    class cFeedDownloader
    {
    public:
      ~cFeedDownloader();

      typedef std::list<cFeedArticle*>::iterator iterator;
      typedef std::list<cFeedArticle*>::const_iterator const_iterator;

      const_iterator ArticlesBegin() { return articles.begin(); }
      const const_iterator ArticlesEnd() const { return articles.end(); }

      void DownloadFeed(const string_t& sURL);

    private:
      void ParseAtomFeed(const std::string& sContent);
      void Clear();

      std::list<cFeedArticle*> articles;
    };

    inline cFeedDownloader::~cFeedDownloader()
    {
      Clear();
    }

    inline void cFeedDownloader::Clear()
    {
      iterator iter(articles.begin());
      const iterator iterEnd(articles.end());
      while (iter != iterEnd) {
        SAFE_DELETE(*iter);
        iter++;
      }

      articles.clear();
    }
  }
}

#endif // CFEED_H
