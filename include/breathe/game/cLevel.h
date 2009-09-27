#ifndef CLEVEL_H
#define CLEVEL_H

#include <breathe/util/base.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/model/cModel.h>

namespace breathe
{
  enum MATERIAL
  {
    MATERIAL_ASPHALT = 0,
    MATERIAL_CONCRETE,
    MATERIAL_GRAVEL,
    MATERIAL_DIRT,
    MATERIAL_GRASS,
    MATERIAL_RUBBER,
    MATERIAL_METAL,
    MATERIAL_SNOW,
    MATERIAL_WATER,
    MATERIAL_AIR,

    MATERIAL_MAX,

    MATERIAL_DEFAULT = MATERIAL_ASPHALT,
    MATERIAL_UNKNOWN = MATERIAL_AIR
  };

  inline size_t MaterialToIndex(MATERIAL material)
  {
    return size_t(material);
  }

  inline MATERIAL IndexToMaterial(size_t index)
  {
    return MATERIAL(index);
  }

  class cSurface
  {
  public:
    MATERIAL material;
    float fFriction;

    render::material::cMaterialRef pMaterial;
  };

  // Gradient is the angle of the road in the usual travelling direction
  // Banking is the angle of the road in the left and right axis

  class cWorld
  {
  public:
    cWorld();

    const float fAirDensity;
    const float fAirConstant;
    const float fGravity;

    // Get the closest surface within epsilon of this point, don't worry about direction of travel
    const cSurface& GetSurfaceAtPoint(const math::cVec3& position);

    // Get the closest surface within epsilon of this point, return the gradient and banking of this surface
    const cSurface& GetSurfaceAtPoint(const math::cVec3& position, const math::cVec3& direction, float& gradient, float& banking);

  private:
    std::vector<cSurface*> surfaces;
  };

  inline cWorld::cWorld() :
    fAirDensity(0.0f),
    fAirConstant(0.0f),
    fGravity(-9.8f)
  {
  }

  class cMission
  {
  public:
    void GetStartingLocation(math::cVec3& currentPosition, math::cVec3& currentRotation);

  private:
    bool bUseCurrentPosition;
    math::cVec3 startingPosition;

    bool bUseCurrentRotation;
    math::cVec3 startingRotation;
  };

  inline void cMission::GetStartingLocation(math::cVec3& currentPosition, math::cVec3& currentRotation)
  {
    if (!bUseCurrentPosition) currentPosition = startingPosition;
    if (!bUseCurrentRotation) currentRotation = startingRotation;
  }


  class cObject;

  namespace render
  {
    class cRender;
    class cTexture;
  }


  namespace vehicle
  {
    class cVehicle2;
    typedef cSmartPtr<cVehicle2> cVehicleRef;
  }

  class cLevel;


  class cLevelSpawn
  {
  public:
    cLevelSpawn();

    math::cVec3 v3Position;
    math::cVec3 v3Rotation;
  };


  const unsigned int NODE_INACTIVE = 0;
  const unsigned int NODE_UNLOAD = 1;
  const unsigned int NODE_ACTIVE = 20; //Seconds since the player has been in the vicinity

  class cLevelNode : public cRenderable
  {
  public:
    cLevelNode(cLevel* p, const string_t& sNewFilename);
    ~cLevelNode() {}

    void Load();
    void Unload();

    void Update(sampletime_t currentTime);
    unsigned int Render();


    unsigned int uiStatus;

    float fFogDistance;

    math::cColour colourFog;

    string_t sFilename;
    string_t sName;
    std::string sCRC;

  private:
    cLevel* pLevel;
  };

  class cLevelLoader
  {
  public:
    explicit cLevelLoader(cLevel& level);

    void Load(const string_t& sFilename);

  private:
    cLevel& level;
  };

  class cLevel : public cUpdateable
  {
  public:
    cLevel();
    ~cLevel();

    bool Load(const string_t& sNewFilename);
    void LoadNode(const string_t& sNewFilename);

    void Update(sampletime_t currentTime);
    unsigned int Render(sampletime_t currentTime);

    cLevelSpawn GetSpawn(); //Get a random spawn
    cLevelSpawn GetSpawn(const math::cVec3& p); //Get closest spawn to requested position


    void AddVehicle(vehicle::cVehicleRef v);
    void RemoveVehicle(vehicle::cVehicleRef v);

    unsigned int RenderVehicles(sampletime_t currentTime, vehicle::cVehicleRef pOwnVehicle);

    vehicle::cVehicleRef FindClosestVehicle(math::cVec3 pos, float fMaxDistance);

    // Global level information loaded from xml
    float fWaterLevel;

    string_t sFilename;

    // Node Information loaded from xml
    unsigned int uiNodeWidth;
    unsigned int uiNodeHeight;
    unsigned int uiNodeHeightMapPixelWidth;
    unsigned int uiNodeHeightMapPixelHeight;

    float fNodeWidth;


    // Dynamic information
    cLevelNode* pCurrentNode;
    unsigned int uiDisplayNodeName;

    //std::map<string_t, render::model::cHeightmap*> mHeightmap;

    std::list<vehicle::cVehicleRef> lVehicle;

    std::vector<cLevelNode*>vNode; //vModel.size()=uiWidth*uiHeight;

    std::vector<cLevelSpawn*>vSpawn;

  private:
    bool LoadXML(const string_t& sNewFilename);

    uint32_t previousTime;
  };
}

extern breathe::cLevel* pLevel;

#endif // CLEVEL_H
