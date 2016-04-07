// Standard headers
#include <cmath>

#include <string>
#include <vector>
#include <list>
#include <map>

// Spitfire headers
#include <spitfire/storage/world_xml.h>

namespace spitfire
{
  cWorldXMLData::~cWorldXMLData()
  {
    Clear();
  }

  void cWorldXMLData::Clear()
  {
    models.clear();
    entities.clear();
  }

  // *** Reader

  cWorldXMLReader::RESULT cWorldXMLReader::ReadFromFile(const string_t& filename)
  {
    xml::reader reader;

    xml::document doc;
    reader.ReadFromFile(doc, filename);

    return RESULT::ERROR_FILE_NOT_FOUND;
  }

  // *** Writer

  cWorldXMLWriter::RESULT cWorldXMLWriter::WriteToFile(const cWorldXMLData& data, const string_t& filename)
  {
    xml::document doc;

    xml::element* world = doc.CreateElement("world");
    doc.AppendChild(world);

    xml::element* cars = doc.CreateElement("cars");
    doc.AppendChild(cars);

    // Cars children
    /*foreach ($cars as $car) {
      xml::element* car = doc.CreateElement("car");
      doc.AppendChild(car);

      car->AddAttribute("id", $cars->id);
    }*/

    /*// Parts node
    $parts_element = $doc->createElement("parts");
    $buy->appendChild($parts_element);

    // Parts children
    foreach ($parts as $part) {
      xml::element* part = doc.CreateElement("part");
      doc.AppendChild(part);

      part->AddAttribute("id", $parts->id);
    }*/

    // Now actually write the xml to filename
    xml::writer writer;
    writer.WriteToFile(doc, filename);

    return RESULT::ERROR_FILE_COULD_NOT_BE_OPENED_FOR_WRITING;
  }
}
