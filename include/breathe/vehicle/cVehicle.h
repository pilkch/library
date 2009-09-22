#ifndef CVEHICLE_H
#define CVEHICLE_H

// The Physics of Racing Series
// http://www.miata.net/sport/Physics/

// The Physics of Racing Series: Part 22 The Magic Formula Lateral Version
// http://www.miata.net/sport/Physics/Part22.html


// Backfiring (Actually called afterfire, backfiring is something else)

// BOV = Blow Off Valve
// Here are the basic reasons for afterfire

// 1. Atmo BOV. Makes you rich for a split second when you shift, all that fuel, not 100% burnt, goes into exh, hits the hot gasses in the exh tract, and explodes.
// 2. Catless or free-er flowing exh on deceleration. I dont understand the mechanics of it, but free flowing exh's make backfire pop noises when decelerating in gear. (all cars will do this with an exh like that)

// SPT exh is freeer flowing, I would imagine you are hearing this on deceleration? Or you have an atmo bov and its just poping in the exh, and you oculdnt hear it with the quiet stock muffler.
// Mine does it all the time. TurboXS turboback exhaust, catless uppipe, stock ECU, stock boost, and stock turbo. On rev-match downshifts I get it; usually don't get too many otherwise.

// Stock engine management uses tons of extra fuel, so it tends to do that a lot. Get a dyno-tune and it will increase power and decrease afterfires. :)

// Get off the throttle and let engine braking take over. i.e. let the engine slow the car down. At first the engine will start to decelerate the car but it gets to a certain speed where it is no longer slowing down the car but sort of evens out to a neutral state. its not slowing or going faster. At that point if you apply enough throttle (not too much to make it accelerate) the car will start to afterfire. I do it all the time. On a Boost gauge its usually at 0. no vaccum or boost when afterfire occurs.



// As it was explained to me, is that what happens is while you are accelerating, small amounts of unburnt and half burnt gas coat the cats, pipes, fiberglass and the walls of the muffler. When you let off the gas the car goes into DFCO mode (decelleration fuel cut off) which cuts all spark and all fuel going into the engine. So now the engine is pumping (oxygen filled) normal air through it. When you have O2 + fuel + heat you get small explosions in the tail pipe. Same reasons apply to the SRT.

// A car backfires because unburned fuel is getting to the muffler. The heat of the exhause system causes the fuel to combust, thus a backfire.



// BMW E36 Race Car:
// "The whine is from the straight cut gears in the transmission versus helical gears used in normal transmissions."



// http://opende.sourceforge.net/wiki/index.php/HOWTO_4_wheel_vehicle

#include <breathe/game/cLevel.h>
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

      float fWeight;
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
    typedef cSmartPtr<cVehicle2> cVehicleRef;

    class cVehicle2 :
      public boost::enable_shared_from_this<cVehicle2>
    {
    private:
      cVehicle2();

    public:
      static cVehicleRef Create();
      ~cVehicle2();

      void Init(cLevelSpawn p, unsigned int uiSeats, physics::cWorld* pWorld);
      void Update(uint32_t currentTime);
      void UpdateInput();

      void PhysicsInit(cLevelSpawn p);
      void PhysicsDestroy();

      void Spawn(cLevelSpawn p); //This is for spawning a car when it first comes into the level

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
