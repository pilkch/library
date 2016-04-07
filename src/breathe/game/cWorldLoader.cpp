#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

#include <breathe/game/cWorldLoader.h>

namespace breathe
{
  namespace game
  {
    // *** cWorldXMLData

    void cWorldXMLData::Clear()
    {
      models.clear();
      entities.clear();
    }



    // *** Reader

    cWorldXMLReader::RESULT cWorldXMLReader::ReadFromFile(const string_t& sFilename, cWorldXMLData& data)
    {
      data.Clear();

      // Make sure that we are loading a .world file
      const string_t sExtension = spitfire::filesystem::GetExtension(sFilename);
      ASSERT(sExtension == TEXT("world"));


      xml::reader reader;

      xml::document doc;
      if (!reader.ReadFromFile(doc, sFilename)) {
        LOG<<"cWorldXMLReader::ReadFromFile \""<<sFilename<<"\" not found"<<std::endl;
        return RESULT_ERROR_FILE_NOT_FOUND;
      }


      breathe::xml::cNode::iterator iterWorld(doc);
      if (!iterWorld.IsValid()) return RESULT_ERROR_FILE_NOT_FOUND;

      iterWorld.FindChild("world");
      if (!iterWorld.IsValid()) return RESULT_ERROR_WORLD_NODE_NOT_FOUND;

      // Load models
      {
        breathe::xml::cNode::iterator iter(iterWorld);

        iter.FindChild("models");
        if (iter.IsValid()) {
          iter.FindChild("model");
          if (iter.IsValid()) {
            while (iter.IsValid()) {
              std::string sModel;
              if (iter.GetAttribute("model", sModel)) {
                LOG<<"Adding Model "<<sModel<<std::endl;
                std::shared_ptr<cWorldModel> pModel(new cWorldModel);

                pModel->model = spitfire::string::ToString_t(sModel);
                iter.GetAttribute("position", pModel->position);
                iter.GetAttribute("rotation", pModel->rotation);

                data.models.push_back(pModel);
              }

              iter.Next("model");
            };
          }
        }
      }

      // Load entities
      {
        breathe::xml::cNode::iterator iter(iterWorld);

        iter.FindChild("entities");
        if (iter.IsValid()) {
          iter.FindChild("entity");
          if (iter.IsValid()) {
            while (iter.IsValid()) {
              std::string sName;
              if (iter.GetAttribute("name", sName)) {
                LOG<<"Adding Entity "<<sName<<std::endl;
                std::shared_ptr<cWorldEntity> pEntity(new cWorldEntity);

                pEntity->name = spitfire::string::ToString_t(sName);
                iter.GetAttribute("position", pEntity->position);
                iter.GetAttribute("rotation", pEntity->rotation);

                // Load properties
                breathe::xml::cNode::iterator iterProperties(iter);
                iterProperties.FindChild("properties");
                if (iterProperties.IsValid()) {
                  iterProperties.FindChild("property");
                  if (iterProperties.IsValid()) {
                    while (iter.IsValid()) {
                      std::string sName;
                      std::string sValue;
                      iterProperties.GetAttribute("name", sName);
                      iterProperties.GetAttribute("value", sValue);

                      pEntity->properties[spitfire::string::ToString_t(sName)] = spitfire::string::ToString_t(sValue);

                      iterProperties.Next("property");
                    }
                  }
                }

                data.entities.push_back(pEntity);
              }

              iter.Next("entity");
            };
          }
        }
      }

      // For testing saving of .world files
      //SaveToFile(TEXT("/media/development/dev/sudoku/data/" ... ".world"));

      return RESULT_SUCCESS;
    }


    // *** Writer

    cWorldXMLWriter::RESULT cWorldXMLWriter::WriteToFile(const string_t& sFilename, const cWorldXMLData& data)
    {
      // Make sure that we are saving a .world file
      const string_t sExtension = spitfire::filesystem::GetExtension(sFilename);
      LOG<<sExtension<<std::endl;
      ASSERT(sExtension == TEXT("world"));


      xml::document doc;

      xml::element* worldElement = doc.CreateElement("world");
      doc.AppendChild(worldElement);

      if (!data.models.empty()) {
        xml::element* modelsElement = doc.CreateElement("models");
        worldElement->AppendChild(modelsElement);

        // Models
        std::list<std::shared_ptr<cWorldModel> >::const_iterator iter = data.models.begin();
        const std::list<std::shared_ptr<cWorldModel> >::const_iterator iterEnd = data.models.end();
        while (iter != iterEnd) {
          xml::element* modelElement = doc.CreateElement("model");
          modelsElement->AppendChild(modelElement);

          modelElement->AddAttribute("model", (*iter)->model);
          modelElement->AddAttribute("position", (*iter)->position);
          modelElement->AddAttribute("rotation", (*iter)->rotation);

          iter++;
        }
      }

      if (!data.entities.empty()) {
        xml::element* entitiesElement = doc.CreateElement("entities");
        worldElement->AppendChild(entitiesElement);

        // Entities
        std::list<std::shared_ptr<cWorldEntity> >::const_iterator iter = data.entities.begin();
        const std::list<std::shared_ptr<cWorldEntity> >::const_iterator iterEnd = data.entities.end();
        while (iter != iterEnd) {
          xml::element* entityElement = doc.CreateElement("entity");
          entitiesElement->AppendChild(entityElement);

          std::shared_ptr<cWorldEntity> pEntity = (*iter);

          entityElement->AddAttribute("name", pEntity->name);
          entityElement->AddAttribute("position", pEntity->position);
          entityElement->AddAttribute("rotation", pEntity->rotation);

          // Add properties
          if (!pEntity->properties.empty()) {
            xml::element* propertiesElement = doc.CreateElement("properties");
            entityElement->AppendChild(propertiesElement);

            std::map<string_t, string_t>::const_iterator iterProperties = pEntity->properties.begin();
            const std::map<string_t, string_t>::const_iterator iterPropertiesEnd = pEntity->properties.end();
            while (iterProperties != iterPropertiesEnd) {
              xml::element* propertyElement = doc.CreateElement("property");
              propertiesElement->AppendChild(propertyElement);

              propertyElement->AddAttribute("name", iterProperties->first);
              propertyElement->AddAttribute("value", iterProperties->second);

              iterProperties++;
            }
          }

          iter++;
        }
      }


      /*if (!data.roads.empty()) {
        <roads>
          <road type="straight" rotation="0" x="12" y="23"/>
        </roads>

        "straight"
        "corner"
        "intersection"
        "t-intersection"
        "cul-de-sac"

        rotation
        "0"
        "1"
        "2"
        "3"
      }*/

      // Now actually write the xml to filename
      xml::writer writer;
      bool bResult = writer.WriteToFile(doc, sFilename);

      return bResult ? RESULT_SUCCESS : RESULT_ERROR_FILE_COULD_NOT_BE_OPENED_FOR_WRITING;
    }
  }
}
