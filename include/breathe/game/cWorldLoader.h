#ifndef CWORLDLOADER_H
#define CWORLDLOADER_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/storage/xml.h>

// TODO: Perhaps in the future something like this
//
//  world.xml
//    |
//  load/save
//    |
//  editor
//    |
//  export
//    |
//  world.bin
//    |
//  import
//    |
//  game
//
// In the meanwhile we just load/save xml in the editor and game

namespace breathe
{
  namespace game
  {
    // These are intermediate classes to hold our world
    // They are purposely not the classes used in the game so that the editor/tools don't actually
    // have to know what to do with each class, you can think of these as a simplified version of everything in breathe/game/
    // It is intentionally not very fast, favouring correctness instead, hence the use of smart pointers and simple types

    /*class cTerrain
    {
    public:
      explicit cTerrain(size_t lengthPerSide);

      string_t textureBase;
      string_t textureDetail;
      cDynamicContainer2D<float> heightmap;
    };

    cTerrain::cTerrain(size_t lengthPerSide) :
      heightmap(lengthPerSide, lengthPerSide)
    {
    }*/

    class cWorldModel
    {
    public:
      string_t model;
      math::cVec3 position;
      math::cQuaternion rotation;
    };

    class cWorldEntity
    {
    public:
      string_t name;
      math::cVec3 position;
      math::cQuaternion rotation;
      std::map<string_t, string_t> properties;
    };

    class cWorldXMLData
    {
    public:
      ~cWorldXMLData() { Clear(); }

      void Clear();

      //cSmartPtr<cWorldTerrain> pTerrain;
      std::list<cSmartPtr<cWorldModel> > models; // NOTE: Models are purely visual
      std::list<cSmartPtr<cWorldEntity> > entities;
    };

    class cWorldXMLReader
    {
    public:
      enum RESULT {
        RESULT_SUCCESS,
        RESULT_ERROR_FILE_NOT_FOUND,
        RESULT_ERROR_WORLD_NODE_NOT_FOUND,
      };

      RESULT ReadFromFile(const string_t& filename, cWorldXMLData& data);
    };

    class cWorldXMLWriter
    {
    public:
      enum RESULT {
        RESULT_SUCCESS,
        RESULT_ERROR_FILE_COULD_NOT_BE_OPENED_FOR_WRITING
      };

      RESULT WriteToFile(const string_t& filename, const cWorldXMLData& data);
    };
  }
}

#endif // CWORLDLOADER_H
