#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <ode/ode.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

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

#include <breathe/game/cLevel.h>
#include <breathe/game/cPlayer.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>

#include <breathe/game/cPetrolBowser.h>
#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cWheel.h>
#include <breathe/vehicle/cSeat.h>
#include <breathe/vehicle/cVehicle.h>

bool fourwheeldrive=false;

float fMaxAcceleration=500.0f;
float fMaxSteer=1.0f;
float fMaxBrake=1.0f;

float fWheelRadius=1.0f;
float fWheelWeight=20.0f;

float fSuspensionK=6.0f;
float fSuspensionU=0.8f;

float fSuspensionMin=0.25f;
float fSuspensionNormal=1.1f;
float fSuspensionMax=1.4f;

//float fSuspensionMin=0.25f;
//float fSuspensionNormal=1.1f;
//float fSuspensionMax=4.4f;

float w=1.6f;
float l=4.8f;
float h=1.0f;

float fMass=1500.0f;

breathe::math::cVec3 v3WheelPos(1.2f, 3.0f, -0.4f);
breathe::math::cVec3 v3GeomPosition(0.0f, 0.0f, -4.0f);

/*TODO:

Add Jaguar XJ220

1.) Road/wheel friction
2.) The physics of turning (related to 1)
3.) Spring/dampener systems for the rays (which act as your suspension)
4.) What to do in case of ray vs physical object collisions

... and there are a lot of other wiggly details that you never think
about until you find yourself staring them down.

They end up being most of the same problems you run into when
attempting to make a "lollipop" style character model (character body
floating over terrain through the aid of a repulsing ray pointed
downwards), if you happen to need that as well.

apply a force in each corner, the magnitude of which is a function
of how deep the ray has penetrated; this function determines your
suspension.

To accelerate, you apply a force in each corner that has a driving
wheel, in the direction of that wheel's "turn".

To steer, you use an anisotropic friction model in a contact joint you
create for the ray intersection, or you just poke the corners of the box
using forces based on wheel heading.

For slip, you can similarly measure sideways versus longitudinal
velocity, and total force applied per corner, to give you fishtailing,
donuts, and all the rest of arcade car racing goodness.





It is not uncommon for people to model their cars as single boxes, not
even using true wheels (cast rays straight down at each of the four
corners, roll your own springy suspension code, place the tires
accordingly along the ray, etc), depending on the number of cars
you'll want in a sim and the sort of handling you want.

Especially in a case like that, how you're going to go about applying
your wind resistance/downforce/upforce/etc is limited only by your own
imagination.  From previous discussions, people have found applying a
down force to greatly help with stability (with some kooky black magic
involving how many tires are touching ground to make sure you're not
accelerating your car into space with this down force when it's in the
process of rolling), as well as making sure to place your center of
mass at the lower edge of the car's box body (that is, translate your
box geom up the Y axis by boxheight/2 relative to the center of the
car, via an nGeomTransform).


You're definately going to want to do a bit of searching of the
archives, there have been quite a few excellent discussions on making
stable cars (all the way from ultra-realistic sim to
good/solid-feeling arcade sim).




Stop rolling over

Implement the same workarounds that have been put in real cars. You'll need to implement
"anti-sway bars". In real vehicles, these were bars that linked each opposite suspension,
so that when one side is pressed down (when going in a curve), it also pressed down on the
suspension on the other side. This prevents the car from tilting over too much.


m_fTorqueOnDriveWheels =
  m_fAccel * (1.0 - m_fClutch) * LookupTorque(m_iRPM) *
  m_fGearRatio[m_iGear] *
  m_fDifferentialRatio;



LIFT and DRAG

D = 0.5 * cD * ro * A * v^2
L = 0.5 * cL * ro * A * v^2

D = drag force
L = lift force
cD = coefficient of drag. it depends basically on the shape of the car.
Production cars have a cD of 0.3 - 0.4. For Formula cars it varies
depending on the tracks: it goes about from 0.7 to 1.3
cL = coefficient of lift. It can be positive (lift) or negative (load).
It's a value around 0.1. In Formula cars it varies from about -2 to -4. If
cL is negative L is called downforce.
ro = air density
A = frontal area of the car (from 1.3 to 1.8 square meters)
v = speed

For an airplane, you want cL to be positive and L to point upwards.
For a race car, you want cL to be negative and L to point downwards :-)






void cPlayer::InitPhysics( CAR *car )
{
  G = 9.8f;
  WHEELINERTIA = 34.0f;
  BRAKE_TORQUE =  5000.0f;
  DRAG         =  0.387f;
  RESISTANCE   = 30.0f;
  CA_R         =  1.2f;
  CA_F         = 1.0f;
  MAX_GRIP     =  10.0f;
  TRACTION_FACTOR = 2.0f;

  std::memset(car, 0, sizeof(CAR) );
  InitCarTypes();
  SetCarType(&cartypes[0], &the_car);

  car->position_wc.x = 0.0f;
  car->position_wc.y = 0.0f;
  car->angle = 0.0001f;
  car->tractionforce = 0;
  car->velocity_wc.x = 0;
  car->velocity_wc.y = 0;
  car->angularvelocity = 0;
  car->steerangle = 0;
  car->fwheelangle = 0;
  car->rwheelangle = 0;
  car->enginetorque = 190; // N.m
  car->rpm = 0;
  car->gear = 1;
  car->accelerator = 0;
  car->brake=0.0f;         // amount of braking (input)  [0 .. 1.0]
  car->handbraking=false;
  car->rwheelav = 0;
  car->wfront = (car->cartype->mass * G * 0.5);
  car->wrear  = (car->cartype->mass * G * 0.5);
  car->pitchangle=0.0f;       // actual pitch angle
  car->aimpitchangle=0.0f;    // destination pitch angle
  car->forcedpitchangle=0.0f; // pitch angle due to wheels being at different heights
  car->rollangle=0.0f;
  car->aimrollangle=0.0f;
  car->forcedrollangle=0.0f;

  car->rwheelav=0.0f;        // rear wheel angular velocity (rad/s)
  car->fwheelav=1.0f;        // front "        "

  car->height=1.0f;          // actual height of car body
  car->aimheight=1.0f;        // target height of body

  for(unsigned int i=0;i<4;i++) car->wheelheight[i]=1.0f; // height for each wheel
}

float cPlayer::LookUpTorque( int rpm )
{
  // flat line 190Nm from 0 to 5000
  // from 5000-6000 rpm the torque drops from 190Nm to zero
  //
  if (rpm < 5000) return 390;
  if (rpm < 6000) return 390*(1000 - (rpm - 5000))*0.001;

  return 0;
}

void cPlayer::GearShifter(CAR *car)
{
  // Automatic gear shifting
  //
  if(automatic)
  {
    if (car->rpm >= 4000 && car->gear < 5 && car->gear != 0 ) car->gear++;
    if (car->rpm <= 2000 && car->gear > 1 ) car->gear--;
  }
}


  cartype->b = 85.0f*UNIT_SCALE; // m
  cartype->c = 60.0f*UNIT_SCALE; // m
  cartype->h = 1.0f;             // m
  cartype->mass = 1500.0f;       // kg
  cartype->inertia =1500.0f;     // kg.m
  cartype->halfwidth = 0.86f;    // m
  cartype->wheelradius = 0.33f;  // m

  cartype->num_gears = 5;
  cartype->gearratio[0] = -3.5f;
  cartype->gearratio[1] = 3.5f;
  cartype->gearratio[2] = 2.2f;
  cartype->gearratio[3] = 1.5f;
  cartype->gearratio[4] = 1.1f;
  cartype->gearratio[5] = 0.94f;
  cartype->differentialratio = 3.6f;
  cartype->gearefficiency = 0.7f;


Garage:

Everything costs money.
Parts are traded in.
Twin turbo part is different part to single turbo part

Inside wheels go slower than outside when turning
Can convert any car to front/rear/all wheel drive simply by buying the correct differential
Front Differential = Front Wheel Drive
Rear Differential = Rear Wheel Drive
Front Differential and Rear Differential = All Wheel Drive

At the Flywheel
The engine produces a torque at the flywheel.
That torque is mechanically increased by the gears in the transmission and/or differential,
resulting in a larger torque on the axle. That axle torque divided by the radius of the tire
equals the force that pushes the car forward

(Supercharger)->(Turbocharger)->Air Filter->Exhaust->Petrol->EngineCapacity->
Transmission->(Rear Differential)->(Front Differential)->Tyres

Weight=chassis + engine + 4*(rim + tyre)

Engine: Inline 4, Inline 4 Cylinder Racing, V6, V6 Racing, V8, V8 Racing, V10 Racing, V12 Racing
Air Filter: Stock, Racing Pod Filter
Performance Kit: Stock, Turbocharger, Twin Turbocharger, Quad Turbocharger, Supercharger
Exhaust: Stock, Cannon, Racing, None
Transmission: Stock 4 Speed Auto, Stock 4 Speed Manual, 5 Speed Manual, 6 Speed Manual Racing
Rims: Stock, Chrome, Racing
Tyres: Stock, Slick, Rally, Racing

Body Kit: Stock, Sport, Racing
Brakes: Stock, Racing
Suspension Stiffness: Stock, Stiff, Soft
Lowering: Stock, High, Low

Paint: Choose RGB (Fills in colour RGB #FFFFFF)


Petrol  (Green Column Graph)
Filling up at Petrol Station: Stock, Premium
Filling up at Backyard/Industrial Garage/Pit Stop: Racing

Burn worst fuel first: fPetrolStock, then fPetrolPremium, then fPetrolRacing.  When fPetrolRacing is empty,
fuel tank is empty.  Stock=0.6f of performance, Premium=0.75f of performance, Racing=1.0f of performance

NOS (Blue Column Graph): Premium, Racing

Inject worst NOS first: fNitrousPremium, then fNitrousRacing.  When fNitrousRacing is empty, fuel tank is empty.
Premium=1.7f of performance, Racing=2.2f of performance










// *** cFlamableTank
// This is a generic tanks for carrying either petrol or nitrous oxide
// It can contain a few different types of the thing it is carrying,
// for petrol: Normal, High Octane, Racing
// for nitrous oxide: Normal, High Octane, Racing
enum TANK_CONTENTS
{
   TANK_CONTENTS_NORMAL = 0,
   TANK_CONTENTS_HIGH_OCTANE,
   TANK_CONTENTS_RACING
};

class cFlamableTank
{
  public:
   void SetTypePetrol(float maxCapacityL);
   void SetTypeNitrousOxide();

   float GetMassKg() const; // This includes contents
   float GetMassNotIncludingContentsKg() const; // This does include contents
   float GetMassOfContentsKg() const; // Contents only

   float GetContentsL(TANK_CONTENTS contentsType) const;

   void EmptyAllContents();
   void EmptyContents(TANK_CONTENTS contentsType);

   bool BurnContentsL(TANK_CONTENTS contentsType, float amountL);

  private:
   bool bIsCarryingPetrol;
   float fContentsLToKg; // For calculating how much a contents weighs
   float fTankMassKg; // How much does this tank weigh empty?

   float fMaxCapacityL; // How much total contents this car can carry in Litres

   math::cVec3 position; // Position relative to the car

   float contentsAmountL[3]; // How much of each type of contents in Litres
};

cFlamableTank::cFlamableTank() :
   position(0.0f, 0.0f, 0.0f)
{
   SetTypePetrol(1.0f);
}

void cFlamableTank::SetTypePetrol(float maxCapacityL)
{
   bIsCarryingPetrol = true;
   fContentsLToKg = 1.0f;
   fMaxCapacityL = maxCapacityL;
   fTankMassKg = fMaxCapacityL * 0.5f;

   contentsAmountL[0] = contentsAmountL[1] = contentsAmountL[2] = 0.0f;
}

void cFlamableTank::SetTypeNitrousOxide()
{
   bIsCarryingPetrol = false;
   fContentsLToKg = 0.1f;
   fMaxCapacityL = 2.0f;
   fTankMassKg = fMaxCapacityL * 0.5f;

   contentsAmountL[0] = contentsAmountL[1] = contentsAmountL[2] = 0.0f;
}

float cFlamableTank::GetMassNotIncludingPetrolKg() const
{
   return fPetrolTankMassKg;
}

float cFlamableTank::GetMassOfPetrolKg() const
{
   float fTotalMassKg = 0.0f;

   // Calculate the amount in Kg and add it to the total
   fTotalMassKg += petrolAmountL[0] * fContentsLToKg;
   fTotalMassKg += petrolAmountL[0] * fContentsLToKg;
   fTotalMassKg += petrolAmountL[0] * fContentsLToKg;

   return fTotalMassKg;
}

float cFlamableTank::GetMassKg() const
{
   return GetMassNotIncludingPetrolKg() + GetMassOfPetrolKg();
}



class cVehicle
{
  public:
   float GetMassBodyKg() const;
   float GetMassEngineKg() const;
   float GetMassPetrolTankKg() const;

   float GetMassNetKg() const; // Total of all components including turbo parts, wheels, crankshaft etc.
   float GetMassGrossKg() const; // GetMassNetKg + petrol + nitrous oxide + passengers

   cVec3 GetCentreOfGravity() const;

  private:
   float fMassKg;

   cEngine engine;
   cPetrolTank petrolTank;
   cVehicleControlModule vcm; // Control module for driver aids for this vehicle
};

cVec3 GetCentreOfGravity() const
{

}

float cVehicle::GetMassBodyKg() const
{
   return fMassKg;
}

float cVehicle::GetMassEngineKg() const
{
   return engine.GetMassKg();
}

float cVehicle::GetMassPetrolTankKg() const
{
   return petrolTank.GetMassKg();
}


// Total of all components including turbo parts, wheels, crankshaft etc.
float cVehicle::GetMassNetKg() const
{
   return GetMassBodyKg() + GetMassEngineKg() + ...;
}

// GetMassNetKg + petrol + nitrous oxide + passengers
float cVehicle::GetMassGrossKg() const
{
   return GetMassNetKg() + petrolTank.GetMassOfPetrolKg() + GetMa
}

*/

namespace breathe
{
  namespace vehicle
  {
    cVehicleRef cVehicle2::Create()
    {
      cVehicleRef ptr(new cVehicle2);
      return ptr;
    }

    cVehicle2::cVehicle2()
    {
      pBody=NULL;
      pMirror=NULL;
      pMetal=NULL;
      pGlass=NULL;
      pWheel=NULL;


      bFourWheelDrive=fourwheeldrive;


      fControl_Accelerate=0.0f;
      fControl_Brake=0.0f;
      fControl_Clutch=0.0f;
      fControl_Steer=0.0f;
      fControl_Handbrake=0.0f;

      fPetrolTankSize=70.0f;
      vPetrolTank.push_back(fPetrolTankSize);


      // http://home.planet.nl/~monstrous/tutcar.html
      properties.fDrag = 0.4257f;      // Drag constant (air resistance)
      properties.fRollResistance = properties.fDrag * properties.fDrag; // Rolling resistance : approximation
      properties.fDownforce = 1.0f; // 1.0f is normal, less than that is lifting off, more is pushing down

      properties.fWeight = 1600.0f;
      properties.fBoost = 1.0f; // 1.0f is standard, 2.0f etc for turbo charged
      properties.fEngineSpeed = 800.0f; // RPM
      properties.fTraction0 = 1.0f;
      properties.fTraction1 = 1.0f;
      properties.fTraction2 = 1.0f;
      properties.fTraction3 = 1.0f;

      properties.vGearRatio.push_back(-1.0f); // Reverse
      properties.vGearRatio.push_back(0.0f); // Neutral
      properties.vGearRatio.push_back(1.0f); // 1st
      properties.vGearRatio.push_back(1.6f); // 2nd
      properties.vGearRatio.push_back(2.3f); // 3rd
      properties.vGearRatio.push_back(3.4f); // 4th
      properties.vGearRatio.push_back(5.0f); // 5th
      properties.iGears = 5; // Number of drive ratios
      properties.iGearCurrent = 1; //-1 = reverse, 0 = Neutral
    }

    cVehicle2::~cVehicle2()
    {
      for (size_t i = 0; i < vWheel.size(); i++) {
        // Deleted in physics list
        //SAFE_DELETE(vWheel[i]);
      };

      for (size_t i = 0; i < vSeat.size(); i++) {
        //SAFE_DELETE(vSeat[i]);
      };

      //PhysicsDestroy();
    }

    void cVehicle2::PhysicsDestroy()
    {
      //physics::RemovePhysicsObject(shared_from_this());
      //RemoveFromWorld();
    }

    void cVehicle2::PhysicsInit(cLevelSpawn p)
    {
      p.v3Position += math::cVec3(0.0f, 0.0f, 2.0f * (fSuspensionMax+fWheelRadius));


      //CreateBox(p.v3Position, p.v3Rotation);

      //physics::AddPhysicsObject(shared_from_this());

      //Rear
      lrWheel_->Init(false, fWheelRadius, fWheelWeight,
        fSuspensionK, fSuspensionU, fSuspensionNormal, fSuspensionMin, fSuspensionMax,
        math::cVec3(-v3WheelPos.x, -v3WheelPos.y, v3WheelPos.z));

      rrWheel_->Init(false, fWheelRadius, fWheelWeight,
        fSuspensionK, fSuspensionU, fSuspensionNormal, fSuspensionMin, fSuspensionMax,
        math::cVec3(v3WheelPos.x, -v3WheelPos.y, v3WheelPos.z));

      //Front
      lfWheel_->Init(true, fWheelRadius, fWheelWeight,
        fSuspensionK, fSuspensionU, fSuspensionNormal, fSuspensionMin, fSuspensionMax,
        math::cVec3(-v3WheelPos.x, v3WheelPos.y, v3WheelPos.z));

      rfWheel_->Init(true, fWheelRadius, fWheelWeight,
        fSuspensionK, fSuspensionU, fSuspensionNormal, fSuspensionMin, fSuspensionMax,
        math::cVec3(v3WheelPos.x, v3WheelPos.y, v3WheelPos.z));
    }

    void cVehicle2::Init(cLevelSpawn p, unsigned int uiSeats, physics::cWorld* pWorld)
    {
      unsigned int i=0;

      for(i=0;i<uiSeats;i++) vSeat.push_back(new cSeat(this));

      for (i=0;i<4;i++) vWheel.push_back(new cWheel(this, pWorld));

      lrWheel_ = vWheel[0];
      rrWheel_ = vWheel[1];
      lfWheel_ = vWheel[2];
      rfWheel_ = vWheel[3];
    }


    void cVehicle2::Update(sampletime_t currentTime)
    {
      return;

      //physics::cPhysicsObject::Update(currentTime);

      /*
      Real rpm = FLT_MIN;

      for (std::vector<Vehicle::Wheel*>::iterator i = b;i != e;i++)
      {
        rpm = std::max(rpm,(*i)->getRPM());
      }

      _engine->update(time);
      Real power = _engine->getPowerAtRPM(rpm);
      Real desired_rpm = _engine->getDesiredRPM();
      Real brake = _engine->getBrakeForce();

      for (std::vector<Vehicle::Wheel*>::iterator i = b;i != e;i++)
      {
        (*i)->update(power,desired_rpm,brake);
      }

      if (_antisway)
      {
        _swayLastUpdate += time;
        if  (_swayLastUpdate > _swayRate)
        {
          applyAntiSwayBarForces ();
          _swayLastUpdate =Ogre::Real(0.0);
        }
      }*/

      //TODO: Run through the parts.  They are already in order in the vector
      //eg. vPart[0]=turbo, vPart[1]=turbo, vPart[2]=tyres
      std::vector<cPart*>::iterator iter = vPart.begin();
      const std::vector<cPart*>::iterator iterEnd = vPart.end();
      while (iter != iterEnd) (*iter++)->Update(currentTime);


      fSteer = fControl_Steer * fMaxSteer;
      fSpeed = fControl_Accelerate * fMaxAcceleration * properties.GetCurrentGearRatio();
      fBrake = (1.0f * fControl_Brake) + (0.1f * fControl_Handbrake);

      if (fBrake > 1.0f) fBrake = 1.0f;

      fBrake *= fMaxBrake;

      //fVel = v.DotProduct(rotation.GetMatrix().GetFront());


      //PHYSICS
      /*
      Basically you cast a ray from the centre position of the wheel(where it should be in its rest position)
      straight down. If the ray hits something and the distance from the contactpoint to the wheel-centre is within
      a certain range(for example 1.0 if the suspension length is 1.0) then apply a force at the wheel-centre using
      the ray's normal to get the force-direction and the distance to calculate the amount of force. Some damping
      has to be applied as well to keep the car from oscillating too much.
      Repeat this for every wheel/ray.

      Basically you make the car hover.

      Then comes the tricky part.
      Applying friction force, driving force and drag.
      Since the car hovers there is no friction at all so you have to fake wheel friction.
      Driving force is a matter of pushing the car from behind and
      apply proper torque on the turning axis to make the car steer.
      Drag should be most simple as you damp the car's velocity based on it's speed.
      */


      size_t i=0;

      for (i=0; i<4; i++) vWheel[i]->Update(currentTime);

      /*
      private void applyAntiSwayBarForces()
      {
        amt=0;

        for (size_t i=0;i<4;i++) {
          Vector3 anchor2 = wheels[i].Joint.Anchor2;
          Vector3 anchor1 = wheels[i].Joint.Anchor;
          Vector3 axis = wheels[i].Joint.Axis2;

          displacement = Vector3.Dot(anchor1-anchor2,axis);

          if (displacement> 0) {
            amt = displacement * swayForce;
            if (amt > swayForceLimit) amt = swayForceLimit;

            wheels[i].Body.AddForce(-axis *amt); //downforce
            wheels[i^1].Body.AddForce(axis *amt); //upforce
          }
        }
      }*/


      /*// drag
      f32 dragConstant = 0.5f;

      neV3 vel = carRigidBody->GetVelocity();

      f32 dot = vel.Dot(body2World.rot[0]);

      neV3 drag = dot * body2World.rot[0] * -dragConstant;

      force += drag;

      controller->SetControllerForce(force);

      controller->SetControllerTorque(torque);*/

//      if (HasBody()) {
        // Aerodynamic Drag
        // To simulate aerodynamic drag, it would be better to use the square of the velocity,
        // because that's how drag works in reality. The force should also be applied at the object's
        // aerodynamic center rather than at the body location. These may be the same, but
        // (especially if you're using geometry transforms) they may not be. The center of
        // the object's cross section would probably be close to the aerodynamic center, at least
        // for non-engineering purposes. In any case, dBodyAddForceAtRelPos? will allow you to
        // apply the force at a specific point in the body's frame of reference.

        // TODO: Use a lot more drag in water
        //float fDampTorque = 1.0f;
        //float fDampLinearVel = 1.0f;
        //dReal const * av = dBodyGetAngularVel(GetBody());
        //dReal const * lv = dBodyGetLinearVel(GetBody());

        // TODO: Check whether we are on our roof/side too
        //dBodyAddTorque( body, -av[0]*av[0]*fDampTorque, -av[1]*av[1]*fDampTorque, -av[2]*av[2]*fDampTorque );
        //dBodyAddForce( body, -lv[0]*lv[0]*fDampLinearVel, -lv[1]*lv[1]*fDampLinearVel, -lv[2]*lv[2]*fDampLinearVel );
  //    }
    }

    void cVehicle2::FillUp(cPetrolBowser *pBowser)
    {
      float fSpace = fPetrolTankSize;
      for (size_t i=0;i<PETROL_SIZE;i++) fSpace-=static_cast<unsigned int>(vPetrolTank[i]);

      cPlayer *p = vSeat[0]->pPlayer;

      if(pBowser->fPrice * fSpace < p->fDollars) {
        vPetrolTank[pBowser->uiType] += fSpace;
        p->fDollars -= pBowser->fPrice * fSpace;
      } else {
        vPetrolTank[pBowser->uiType] += (p->fDollars / pBowser->fPrice);
        p->fDollars=0.0f;
      }
    }

    void cVehicle2::UpdateInput()
    {
      cPlayer *pPlayer=vSeat[0]->pPlayer;
      if (nullptr == pPlayer) return;

      if (pPlayer->bInputClutch) {
        fControl_Clutch += 0.1f;
        if(fControl_Clutch > 1.0f) fControl_Clutch = 1.0f;
      } else fControl_Clutch *= 0.9f;


      fControl_Accelerate = pPlayer->fInputUp;
      fControl_Brake = pPlayer->fInputDown;
      if(pPlayer->fInputLeft > pPlayer->fInputRight) fControl_Steer = -1.0f * pPlayer->fInputLeft;
      else fControl_Steer = pPlayer->fInputRight;

      // Instant on
      if (pPlayer->bInputHandbrake) fControl_Handbrake = 1.0f;
      else fControl_Handbrake = 0.0f;
    }

    void cVehicle2::AssignPlayer(cPlayer *p)
    {
      const size_t n = vSeat.size();
      for(size_t i = 0;i<n;i++) {
        cSeat* s = vSeat[i];

        if(!s->pPlayer) {
          vSeat[i]->AssignPlayer(p);
          return;
        }
      }
    }
  }
}

/*
class cVehicle
{
  public:
    void ConvertCarToWorldOrientation(const math::cVec3& from0, math::cVec3& toFinal) const { body->ConvertBodyToWorldOrientation(from0, toFinal); }
   // Convert orientation 'from' from car coordinates (local)
   // to world coordinates (into 'to')
   // Order is reverse to YPR; RPY here (ZXY)

    void ConvertCarToWorldCoords(const math::cVec3& from, math::cVec3& toFinal) const { body->ConvertBodyToWorldPos(from0, toFinal); }
   // Convert vector 'from' from car coordinates (local)
   // to world coordinates (into 'to')
   // Should be used for coordinates, NOT orientations (a translation
   // is used here as well as a rotation)

    void ConvertWorldToCarOrientation(const math::cVec3& from0, math::cVec3& toFinal) const { body->ConvertWorldToBodyOrientation(from0, toFinal); }
   // Convert vector 'from' from world coordinates (global)
   // to car coordinates (into 'to')

    void ConvertWorldToCarCoords(const math::cVec3& from, math::cVec3& toFinal) const { body->ConvertWorldToBodyPos(from0, toFinal); }
   // Convert vector 'from' from car coordinates (local)
   // to world coordinates (into 'to')
   // Should be used for coordinates, NOT orientations (a translation
   // is used here as well as a rotation)

  private:
    void CalculateForces();
    void ApplyForces();

    void ApplyRotations();
    void ApplyAcceleration();
};

void cVehicle::CalculateForces()
{
  engine->CalcForces();
  for (size_t i = 0;i < wheels; i++) susp[i]->CalcForces();
  for (size_t i = 0;i < wheels; i++) wheel[i]->CalcForces();

  body->CalcForces();
}

void cVehicle::ApplyForces()
{
  engine->ApplyForces();
  for (size_t i = 0; i < wheels; i++) wheel[i]->ApplyForces();
  body->ApplyForces();
}

void cVehicle::ApplyRotations()
{
  body->ApplyRotations();
}

// Look at the wheel forces and accelerate the car with the sum
void cVehicle::ApplyAcceleration()
{
}

void cVehicle::Update()
{
  //
  // Calculate state of vehicle before calculating forces
  //
  // Calculate all forces on the car
  //
  CalculateForces();

  //
  // Now that all forces have been generated, apply
  // them to get accelerations (translational/rotational)
  //
  ApplyForces();

  // Rotational dynamics; 3 axes of rotation, 6 DOF
  ApplyRotations();
  ApplyAcceleration();


   // After all forces & accelerations are done, deduce
  // data for statistics


  // Engine RPM is related to the rotation of the powered wheels,
  // since they are physically connected, somewhat
  // Take the minimal rotation
  float_t minV=99999.0f;
  float_t maxV = 0;
  for (size_t i=0;i<wheels;i++)
    if (wheel[i]->IsPowered()) {
    if (wheel[i]->GetRotationV() > maxV) maxV=wheel[i]->GetRotationV();
    if (wheel[i]->GetRotationV() < minV) minV=wheel[i]->GetRotationV();
//qdbg("  rpm: wheel%d: rotV=%f\n",i,wheel[i]->GetRotationV());
    }

    engine->ApplyWheelRotation(maxV);

  // Remember old location of car to check for hitting things
    math::cVec3 timingPosBC,timingPosPreWC,timingPosPostWC;
    timingPosBC.SetToZero();
    body->ConvertBodyToWorldPos(&timingPosBC,&timingPosPreWC);

  // Integrate step for all parts
    body->Integrate();
    engine->Integrate();
    steer->Integrate();
    for (i=0;i<wheels;i++) wheel[i]->Integrate();
    for (i=0;i<wheels;i++) susp[i]->Integrate();

  // Check if something was hit/crossed
    body->ConvertBodyToWorldPos(&timingPosBC,&timingPosPostWC);

    int ctl = RMGR->scene->curTimeLine[index];
    if (RMGR->trackVRML->timeLine[ctl]->CrossesPlane(&timingPosPreWC, &timingPosPostWC)) {
    // Notify scene of our achievement
      RMGR->scene->TimeLineCrossed(this);
    }
}
*/
