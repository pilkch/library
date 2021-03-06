#ifndef WORLD_H
#define WORLD_H

#include <spitfire/spitfire.h>

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

namespace spitfire
{
  // These are intermediate classes to hold our world
  // They are purposely not the classes used in the game so that the editor/tools don't actually
  // have to know what to do with each class, you can think of these as a simplified version of everything in spitfire/game/
  // It is intentionally not very fast, favouring correctness instead, hence the use of smart pointers and simple types

  class cTerrain
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

    std::shared_ptr<cTerrain> pTerrain;
    std::list<std::shared_ptr<cModel> > models; // NOTE: Models are purely visual
    std::list<std::shared_ptr<cEntity> > entities;
  };

  class cWorldXMLReader
  {
  public:
    enum class RESULT {
      SUCCESS,
      ERROR_FILE_NOT_FOUND,
      ERROR_FILE_DOES_NOT_HAVE_INSERT_CRITICAL_FEATURE_HERE
    };

    RESULT ReadFromFile(const string_t& filename);

    const cWorldXMLData& GetData() const { return data; }

  private:
    cWorldXMLData data;
  };

  class cWorldXMLWriter
  {
  public:
    enum class RESULT {
      SUCCESS,
      ERROR_FILE_COULD_NOT_BE_OPENED_FOR_WRITING
    };

    RESULT WriteToFile(const cWorldXMLData& data, const string_t& filename);
  };
}

#endif // WORLD_H
