#ifndef CVEHICLE_H
#define CVEHICLE_H

namespace breathe
{
	class c3ds;

	namespace vehicle
	{
		class cWheel;
		class cSeat;

		//Properties, change based on parts installed
		class cVehicleProperties
		{
		public:
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
			

			float GetCurrentGearRatio()
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
		};

		class cVehicle : virtual public physics::cPhysicsObject
		{
		public:
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

			std::vector<cWheel *>vWheel;

			cWheel *lfWheel_;
			cWheel *rfWheel_;
			cWheel *lrWheel_;
			cWheel *rrWheel_;

			c3ds *pBody, *pMirror, *pMetal, *pGlass, *pWheel;

			std::vector<cSeat*> vSeat;

			cVehicle();
			~cVehicle();

			void Init(cLevelSpawn p, unsigned int uiSeats);
			void Update(float fCurrentTime);
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
