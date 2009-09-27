#ifndef CFEED_H
#define CFEED_H

#include <spitfire/util/datetime.h>

#include <breathe/breathe.h>

namespace breathe
{
  namespace network
  {
    // This is a feed reader for reading Atom feeds
    // We do not read RSS feeds or even complex encoding in Atom feeds
    // It is basically the bare minimum required to read an update atom feed that we have authored ourselves

    class cFeedArticleLink
    {
    public:
      std::string rel;
      std::string type;
      std::string href;
    };

    class cFeedArticleContent
    {
    public:
      std::string type;
      std::string src;
    };

    class cFeedArticle
    {
    public:
      friend class cFeed;

      const std::string& GetID() const { return id; }
      //const util::cDateTime& GetDate() const { return date; }
      const std::string& GetDate() const { return date; }
      const std::string& GetTitle() const { return title; }
      const std::string& GetSummary() const { return summary; }

    private:
      std::string id;
      //util::cDateTime date;
      std::string date;
      std::string title;
      std::string summary;

      cFeedArticleLink link; // TODO: Do we ever get more than one of these?
      cFeedArticleContent content; // TODO: Do we ever get more than one of these?
    };

    class cFeed
    {
    public:
      cFeed();
      ~cFeed();

      enum TYPE
      {
        TYPE_ATOM_2005,
        TYPE_DEFAULT = TYPE_ATOM_2005
      };

      TYPE GetType() const { return type; }
      const std::string& GetTitle() const { return title; }

      typedef std::list<cFeedArticle*>::iterator iterator;
      typedef std::list<cFeedArticle*>::const_iterator const_iterator;

      const_iterator ArticlesBegin() { return articles.begin(); }
      const const_iterator ArticlesEnd() const { return articles.end(); }

      void DownloadFeed(const string_t& sURL);

    private:
      void ParseAtomFeed(const std::string& sContent);
      void Clear();

      TYPE type;
      std::string title;
      std::list<cFeedArticle*> articles;
    };

    inline cFeed::cFeed() :
      type(TYPE_ATOM_2005)
    {
    }

    inline cFeed::~cFeed()
    {
      Clear();
    }

    inline void cFeed::Clear()
    {
      type = TYPE_DEFAULT;
      title.clear();

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
