#ifndef CVEHICLE_H
#define CVEHICLE_H

namespace BREATHE
{
	class c3ds;

	namespace VEHICLE
	{
		class cWheel;
		class cSeat;

		class cVehicle : virtual public PHYSICS::cPhysicsObject
		{
		public:
			PHYSICS::cPhysics *pPhysics;

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

			float fPetrolTankSize; //Total size

			std::vector<float>vPetrolTank;

			//Properties, change based on parts installed
			struct struct_properties
			{
				float fDrag;
				float fDownforce;
				float fWeight;
				float fBoost;
				float fEngineSpeed;
				float fTraction0;
				float fTraction1;
				float fTraction2;
				float fTraction3;
			}properties;

			std::vector<cPart*>vPart;

			std::vector<cWheel *>vWheel;

			cWheel *lfWheel_;
			cWheel *rfWheel_;
			cWheel *lrWheel_;
			cWheel *rrWheel_;

			c3ds *pBody, *pMirror, *pMetal, *pGlass, *pWheel;

			std::vector<cSeat*> vSeat;

			cVehicle(PHYSICS::cPhysics *p);
			~cVehicle();

			void Init(cLevelSpawn p, unsigned int uiSeats);
			void Update(float fTimeStep);
			void UpdateInput();

			void PhysicsInit(cLevelSpawn p);
			void PhysicsDestroy();

			void Spawn(cLevelSpawn p); //This is for spawning a car when it first comes into the level

			void FillUp(cPetrolBowser *pBowser);

			void AssignPlayer(cPlayer *p);
		};
	}
}

#endif //CVEHICLE_H
