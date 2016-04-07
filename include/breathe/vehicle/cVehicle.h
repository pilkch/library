#ifndef CVEHICLE_H
#define CVEHICLE_H

#include <breathe/game/component.h>

namespace breathe
{
  namespace vehicle
  {
    class cVehicle
    {
    public:
      void Create(breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture);

      breathe::game::cGameObjectRef GetGameObject() { return pGameObject; }
      breathe::render::cTextureFrameBufferObjectRef GetCubeMapTexture() { return pCubeMapTexture; }

    private:
      breathe::game::cGameObjectRef pGameObject;
      breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture;
    };


    class cVehicleFactory
    {
    public:
      void CreateCar(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const;
      //void CreatePlane(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const;
      void CreateHelicopter(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const;
    };
  }
}



#include <breathe/game/cPetrolBowser.h>
#include <breathe/game/cPlayer.h>

#include <breathe/vehicle/cPart.h>

#ifdef BUILD_PHYSICS_3D
#include <breathe/physics/physics.h>
#endif

namespace breathe
{
  class c3ds;

  namespace vehicle
  {
    typedef uint32_t id_t;

    class cWheel;
    class cSeat;

    //Properties, change based on parts installed
    class cVehicleProperties
    {
    public:
      float GetCurrentGearRatio() const
      {
        if((iGearCurrent >= -1) && (iGearCurrent < iGears + 2))
          return vGearRatio[1 + iGearCurrent];
        else
          return 1.0f;
      }

      void ShiftUp()
      {
        iGearCurrent++;
        if(iGearCurrent > iGears) iGearCurrent = iGears;
      }

      void ShiftDown()
      {
        iGearCurrent--;
        if(iGearCurrent < -1) iGearCurrent = -1;
      }


      float fDrag;
      float fRollResistance;
      float fDownforce;

      float fMassKg;
      float fBoost;
      float fEngineSpeed;
      float fTraction0;
      float fTraction1;
      float fTraction2;
      float fTraction3;

      std::vector<float>vGearRatio; // 0=Reverse, 1=Neutral, 2=1st, 3=2nd, ...
      int iGears; // Number of drive ratios
      int iGearCurrent; // -1 = reverse, 0 = Neutral

      math::cColour bodyColourDiffuse;
      //math::cColour bodyColourSpecular;
    };

    class cVehicle2;
    typedef std::shared_ptr<cVehicle2> cVehicleRef;

    class cVehicle2 :
      public std::enable_shared_from_this<cVehicle2>
    {
    private:
      cVehicle2();

    public:
      static cVehicleRef Create();
      ~cVehicle2();

      void Init(const math::cVec3& position, unsigned int uiSeats, physics::cWorld* pWorld);
      void Update(uint32_t currentTime);
      void UpdateInput();

      void PhysicsInit(const math::cVec3& position);
      void PhysicsDestroy();

      void Spawn(const math::cVec3& position); //This is for spawning a car when it first comes into the level

      void FillUp(cPetrolBowser* pBowser);

      void AssignPlayer(cPlayer* p);


      id_t uniqueID;

      bool bEngineOff;
      bool bFourWheelDrive;

      //0.0f..1.0f control inputs
      float fControl_Accelerate;
      float fControl_Brake;
      float fControl_Steer; //- = left, + = right
      float fControl_Handbrake;
      float fControl_Clutch;

      //Actual speed
      float fSpeed;
      float fSteer;
      float fBrake;
      float fVel;

      cVehicleProperties properties;

      float fPetrolTankSize; //Total size
      std::vector<float>vPetrolTank;

      std::vector<cPart*>vPart;

      std::vector<cWheel*>vWheel;

      cWheel* lfWheel_;
      cWheel* rfWheel_;
      cWheel* lrWheel_;
      cWheel* rrWheel_;

      c3ds* pBody;
      c3ds* pMirror;
      c3ds* *pMetal;
      c3ds* *pGlass;
      c3ds* *pWheel;

      std::vector<cSeat*> vSeat;
    };

    /*// partsUnsorted is the collection of all parts
    // partsSortedEngineToWheels is the collection of parts that go from the engine to the wheels
    // Do not save or load anything from or to partsSortedEngineToWheels as it is not a complete list of the parts
    class cVehicle
    {
    public:
      void AddPart(cVehiclePart* part);
      void RemovePart(cVehiclePart* part);

      const std::list<cVehiclePart*>& GetPartList() const { return partsUnsorted; }

    private:
      void Sort();

      std::list<cVehiclePart*> partsUnsorted;

      // For internal use only
      std::list<cVehiclePart*> partsSortedEngineToWheels;
    };

    inline void cVehicle::AddPart(cVehiclePart* part)
    {
      partsUnsorted.push_back(part);
      if (part->IsEngineToWheels()) partsSortedEngineToWheels.push_back(part);

      Sort();
    }

    inline void cVehicle::RemovePart(cVehiclePart* part)
    {
      partsUnsorted.remove(part);
      partsSortedEngineToWheels.remove(part);
    }*/
  }
}

#endif // CVEHICLE_H
