#ifndef CVEHICLEPART_H
#define CVEHICLEPART_H

namespace breathe
{
	class c3ds;

	namespace vehicle
	{
		class cVehicle;

		const int VEHICLEPART_NONE=0;
		const int VEHICLEPART_TURBOCHARGER=1;
		const int VEHICLEPART_SUPERCHARGER=2;

		class cPart
		{
		public:
			unsigned int uiType;
			float fHealth; //Starts at 100.0f and gradually goes down

			cVehicle *pVehicle;

			c3ds *pModelNew;
			c3ds *pModelBroken;

			cPart();

			void Install(cVehicle *p);
			virtual void Update()=0;
		};

		class cPartTurboCharger : public cPart
		{
		public:
			cPartTurboCharger();

			void Install(cVehicle *p);
			void Update();
		};
		
		class cPartSuperCharger : public cPart
		{
		public:
			cPartSuperCharger();

			void Install(cVehicle *p);
			void Update();
		};
	}
}

#endif //CVEHICLEPART_H
