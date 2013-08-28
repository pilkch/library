#ifndef HTML_H
#define HTML_H

// Spitfire headers
#include <spitfire/util/string.h>

namespace spitfire
{
  namespace storage
  {
    class cHTMLDocumentProperties
    {
    public:
      cHTMLDocumentProperties();

      string_t sLanguageCode;
    };

    inline cHTMLDocumentProperties::cHTMLDocumentProperties() :
      sLanguageCode(TEXT("EN"))
    {
    }

    template <class W, class C>
    class cHTMLDocument
    {
    public:
      void Create(W& writer, const C& controller);

    private:
      void Begin(W& writer, const cHTMLDocumentProperties& properties, const C& controller);
      void AddBody(W& writer, const cHTMLDocumentProperties& properties, const C& controller);
      void End(W& writer);
    };

    template <class W, class C>
    inline void cHTMLDocument<W, C>::Create(W& writer, const C& controller)
    {
      cHTMLDocumentProperties properties;
      controller.GetProperties(properties);

      Begin(writer, properties, controller);

      AddBody(writer, properties, controller);

      End(writer);
    }

    template <class W, class C>
    inline void cHTMLDocument<W, C>::Begin(W& writer, const cHTMLDocumentProperties& properties, const C& controller)
    {
      writer.WriteLine("<!DOCTYPE HTML>");
      writer.WriteLine("<html lang=\"" + properties.sLanguageCode + "\">");
      writer.WriteLine("<head>");
      writer.WriteLine("  <meta charset=\"UTF-8\"/>");
      writer.WriteLine("");
      writer.WriteLine("  <title>" + controller.GetTitle() + "</title>");
      writer.WriteLine("");
      writer.WriteLine("  <meta name=\"description\" content=\"" + controller.GetDescription() + "\"/>");
      writer.WriteLine("");
      writer.WriteLine("  <!-- Define a viewport to mobile devices to use - telling the browser to assume that the page is as wide as the device (width=device-width) and setting the initial page zoom level to be 1 (initial-scale=1.0) -->");
      writer.WriteLine("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>");
      writer.WriteLine("");
      writer.WriteLine("  <!-- Add normalize.css which enables browsers to render all elements more consistently and in line with modern standards as it only targets particular styles that need normalizing -->");
      writer.WriteLine("  <link href=\"css/normalize.css\" rel=\"stylesheet\" media=\"all\"/>");
      writer.WriteLine("");
      writer.WriteLine("  <!-- Include the site stylesheet -->");
      writer.WriteLine("  <link href=\"css/styles.css\" rel=\"stylesheet\" media=\"all\"/>");
      writer.WriteLine("");
      writer.WriteLine("  <style type = \"text/css\">");
      writer.WriteLine("    .table_border {");
      writer.WriteLine("      border: 1px solid black;");
      writer.WriteLine("    }");
      writer.WriteLine("    .table_heading {");
      writer.WriteLine("      font-weight:bold;");
      writer.WriteLine("    }");
      writer.WriteLine("  </style>");

      // Add external javascripts
      std::vector<std::string> scripts = controller.GetExternalJavaScripts();
      const size_t n = scripts.size();
      for (size_t i = 0; i < n; i++) {
        writer.WriteLine("  <script language=\"JavaScript\" src=\"" + scripts[i] + "\"></script>");
      }

      writer.WriteLine("</head>");
      writer.WriteLine("<body>");
    }

    template <class W, class C>
    inline void cHTMLDocument<W, C>::AddBody(W& writer, const cHTMLDocumentProperties& properties, const C& controller)
    {
      const std::vector<std::pair<std::string, std::string> > articles = controller.GetArticles();

      // Write the banner if we have a valid banner title
      if (!controller.GetBannerTitle().empty()) {
        // Start our banner
        writer.WriteLine("  <header role=\"banner\">");
        writer.WriteLine("    <h1>" + controller.GetBannerTitle() + "</h1>");

        // TODO: This should be more like a site map, navigation links to the rest of the site, not this page
        /*if (!articles.empty()) {
          writer.WriteLine("");
          writer.WriteLine("    <nav role=\"navigation\">");
          writer.WriteLine("      <ul>");

          // Add a link for each article
          const size_t nArticles = articles.size();
          for (size_t i = 0; i < nArticles; i++) {
            writer.WriteLine("        <li><a href=\"#" + articles[i].first + "\">" + articles[i].second + "</a></li>");
          }

          writer.WriteLine("      </ul>");
          writer.WriteLine("    </nav>");
        }*/

        // Finish our banner
        writer.WriteLine("  </header>");
      }

      // Start our main div
      writer.WriteLine("");
      writer.WriteLine("  <div class=\"wrap\">");
      writer.WriteLine("");
      writer.WriteLine("    <!-- The <main> element is used to enclose the main content, i.e. that which contains the central topic of a document -->");
      writer.WriteLine("    <main role=\"main\">");
      writer.WriteLine("      <section>");

      // Add the header if we have a valid section title
      if (!controller.GetSectionTitle().empty()) {
        writer.WriteLine("        <header>");
        writer.WriteLine("          <h2>" + controller.GetSectionTitle() + "</h2>");

        // Add the links to the articles if we have any
        if (!articles.empty()) {
          writer.WriteLine("          <nav role=\"navigation\">");
          writer.WriteLine("            <ul>");

          // Add a link for each article
          const size_t nArticles = articles.size();
          for (size_t i = 0; i < nArticles; i++) {
            writer.WriteLine("              <li><a href=\"#" + articles[i].first + "\">" + articles[i].second + "</a></li>");
          }

          writer.WriteLine("            </ul>");
          writer.WriteLine("          </nav>");
        }

        writer.WriteLine("        </header>");
      }

      if (!articles.empty()) {
        const size_t nArticles = articles.size();
        for (size_t i = 0; i < nArticles; i++) {
          // Add the article
          writer.WriteLine("");
          writer.WriteLine("        <article id=\"" + articles[i].first + "\">");
          writer.WriteLine("          <h3>" + articles[i].second + "</h3>");
          writer.WriteLine("");
          controller.AddArticle(writer, articles[i].first);
          writer.WriteLine("        </article>");
        }
      }

      writer.WriteLine("      </section>");
      writer.WriteLine("    </main>");


      // Add asides
      const std::vector<std::pair<std::string, std::string> > asides = controller.GetAsides();
      if (!asides.empty()) {
        const size_t nAsides = asides.size();
        for (size_t i = 0; i < nAsides; i++) {
          // Add the aside
          writer.WriteLine("    <aside role=\"complementary\">");
          writer.WriteLine("      <h3>" + asides[i].second + "</h3>");
          writer.WriteLine("");
          controller.AddAside(writer, asides[i].first);
          writer.WriteLine("    </aside>");
        }
      }

      // End our main div
      writer.WriteLine("  </div>");

    }

    template <class W, class C>
    inline void cHTMLDocument<W, C>::End(W& writer)
    {
      writer.WriteLine("");
      writer.WriteLine("  <footer role=\"contentinfo\">");
      writer.WriteLine("    <!-- Copyright information -->");
      writer.WriteLine("    <small>Copyright &copy; <time datetime=\"2013\">2013</time></small>");
      writer.WriteLine("  </footer>");
      writer.WriteLine("</body>");
      writer.WriteLine("</html>");
    }
  }
}

#endif // HTML_H
