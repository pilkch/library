#ifndef WORLD_H
#define WORLD_H

#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cQuaternion.h>

#include <breathe/algorithm/algorithm.h>

#include <breathe/storage/xml.h>

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
  // These are intermediate classes to hold our world
  // They are purposely not the classes used in the game so that the editor/tools don't actually
  // have to know what to do with each class, you can think of these as a simplified version of everything in breathe/game/
  // It is intentionally not very fast, favouring correctness instead, hence the use of smart pointers and simple types

  class cTerrain
  {
  public:
    explicit cTerrain(size_t lengthPerSide);

    string_t textureBase;
    string_t textureDetail;
    breathe::cDynamicContainer2D<float> heightmap;
  };

  cTerrain::cTerrain(size_t lengthPerSide) :
    heightmap(lengthPerSide, lengthPerSide)
  {
  }

  class cModel
  {
  public:
    math::cVec3 position;
    math::cQuaternion rotation;

    string_t model;
  };

  class cEntity
  {
  public:
    math::cVec3 position;
    math::cQuaternion rotation;

    string_t name;
    std::map<string_t, string_t> attributes;
  };

  class cWorldXMLData
  {
  public:
    ~cWorldXMLData();

    void Clear();

    cSmartPtr<cTerrain> pTerrain;
    std::list<cSmartPtr<cModel> > models; // NOTE: Models are purely visual
    std::list<cSmartPtr<cEntity> > entities;
  };

  class cWorldXMLReader
  {
  public:
    enum RESULT {
      RESULT_SUCCESS,
      RESULT_ERROR_FILE_NOT_FOUND,
      RESULT_ERROR_FILE_DOES_NOT_HAVE_INSERT_CRITICAL_FEATURE_HERE
    };

    RESULT ReadFromFile(const string_t& filename);

    const cWorldXMLData& GetData() const { return data; }

  private:
    cWorldXMLData data;
  };

  class cWorldXMLWriter
  {
  public:
    enum RESULT {
      RESULT_SUCCESS,
      RESULT_ERROR_FILE_COULD_NOT_BE_OPENED_FOR_WRITING
    };

    RESULT WriteToFile(const cWorldXMLData& data, const string_t& filename);
  };
}

#endif // WORLD_H
