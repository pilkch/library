#ifndef CDOWNLOADHTTP_H
#define CDOWNLOADHTTP_H

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

      const cDateTime& GetDate() const { return date; }
      const std::string& GetTitle() const { return title; }
      const std::string& GetContent() const { return content; }

    private:
      NO_COPY(cFeedArticle);

      std::string title;
      std::string url;
      std::string uniqueid;
      cDateTime date;
      std::string title;
      std::string content;
    };

    class cFeedDownloader
    {
    public:
      ~cFeedDownloader();

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
      for each article SAFE_DELETE();
    }
  }
}
