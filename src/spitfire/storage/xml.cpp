#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

// Writing to and from text files
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#include <spitfire/storage/xml.h>

namespace spitfire
{
  namespace xml
  {
    // ** reader

    util::PROCESS_RESULT reader::ReadFromFile(util::cProcessInterface& interface, document::cDocument& doc, const string_t& filename) const
    {
      doc.Clear();

      return doc.LoadFromFile(interface, filename);
    }

    util::PROCESS_RESULT reader::ReadFromString(util::cProcessInterface& interface, document::cDocument& doc, const std::string& content) const
    {
      doc.Clear();

      return doc.LoadFromString(interface, content);
    }


    // ** writer

    bool writer::WriteToFile(const document::cDocument& doc, const string_t& filename) const
    {
      return doc.SaveToFile(filename);
    }
  }
}
