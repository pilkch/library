#include <spitfire/storage/html.h>

//#ifdef BUILD_UNITTESTS

#include <spitfire/storage/file.h>


// ** cMyController

class cMyController
{
public:
  void GetProperties(spitfire::storage::cHTMLDocumentProperties& properties) const;

  spitfire::string_t GetTitle() const;
  spitfire::string_t GetDescription() const;
  spitfire::string_t GetBannerTitle() const;
  spitfire::string_t GetSectionTitle() const;

  std::vector<std::pair<std::string, std::string> > GetArticles() const;
  template <class W>
  void AddArticle(W& writer, const std::string& sArticle) const;

  std::vector<std::pair<std::string, std::string> > GetAsides() const;
  template <class W>
  void AddAside(W& writer, const std::string& sAside) const;
};

void cMyController::GetProperties(spitfire::storage::cHTMLDocumentProperties& properties) const
{
  properties.sLanguageCode = TEXT("EN");
}

spitfire::string_t cMyController::GetTitle() const
{
  return TEXT("My Title");
}

spitfire::string_t cMyController::GetDescription() const
{
  return TEXT("My Description");
}

spitfire::string_t cMyController::GetBannerTitle() const
{
  return TEXT("My Banner");
}

spitfire::string_t cMyController::GetSectionTitle() const
{
  return TEXT("My Section");
}

std::vector<std::pair<std::string, std::string> > cMyController::GetArticles() const
{
  std::vector<std::pair<std::string, std::string> > articles;
  articles.push_back(std::make_pair(TEXT("myarticle"), TEXT("My Article")));
  return articles;
}

template <class W>
void cMyController::AddArticle(W& writer, const std::string& sArticle) const
{
  if (sArticle == TEXT("myarticle")) {
    writer.WriteLine("<ol>");
    writer.WriteLine("  <li>Read the comments in this template</li>");
    writer.WriteLine("  <li>Decide how you think your content may fit into the template</li>");
    writer.WriteLine("  <li>Start building your document</li>");
    writer.WriteLine("</ol>");
    writer.WriteLine("");
    writer.WriteLine("<table class=\"table_border\">");
    writer.WriteLine("  <tr class=\"table_heading\">");
    writer.WriteLine("    <th class=\"table_border\">Name</th>");
    writer.WriteLine("    <th class=\"table_border\">Gender</th>");
    writer.WriteLine("    <th class=\"table_border\">Age</th>");
    writer.WriteLine("  </tr>");
    writer.WriteLine("");
    writer.WriteLine("  <tr>");
    writer.WriteLine("    <td>Chris</td>");
    writer.WriteLine("    <td>Male</td>");
    writer.WriteLine("    <td>25</td>");
    writer.WriteLine("  </tr>");
    writer.WriteLine("");
    writer.WriteLine("  <tr>");
    writer.WriteLine("    <td class=\"table_border\">Tina</td>");
    writer.WriteLine("    <td class=\"table_border\">Female</td>");
    writer.WriteLine("    <td class=\"table_border\">30</td>");
    writer.WriteLine("  </tr>");
    writer.WriteLine("</table>");
  }
}

std::vector<std::pair<std::string, std::string> > cMyController::GetAsides() const
{
  std::vector<std::pair<std::string, std::string> > asides;
  asides.push_back(std::make_pair(TEXT("myaside"), TEXT("My Aside")));
  return asides;
}

template <class W>
void cMyController::AddAside(W& writer, const std::string& sAside) const
{
  if (sAside == TEXT("myaside")) {
    writer.WriteLine("      <p>This is my aside. Have an article: <a href=\"http://www.iandevlin.com/blog/2011/04/html5/html5-section-or-article\">section or article?</a>.</p>");
  }
}


class cMyWriter
{
public:
  explicit cMyWriter(const spitfire::string_t& sFilePath);

  void WriteLine(const std::string& sLine);

private:
  spitfire::storage::cWriteFile file;
};

cMyWriter::cMyWriter(const spitfire::string_t& sFilePath) :
  file(sFilePath)
{
}

void cMyWriter::WriteLine(const std::string& sLine)
{
  file.WriteLineUTF8LF(sLine);
}


void TestHTML()
{
  cMyController controller;

  cMyWriter writer(TEXT("/home/chris/index.html"));

  spitfire::storage::cHTMLDocument<cMyWriter, cMyController> document;

  document.Create(writer, controller);
}

//#endif // BUILD_UNITTESTS
