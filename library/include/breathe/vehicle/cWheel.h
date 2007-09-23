#ifndef CVEHICLEWHEEL_H
#define CVEHICLEWHEEL_H

namespace BREATHE
{
	namespace VEHICLE
	{
		class cVehicle;

		class cWheel : virtual public PHYSICS::cPhysicsObject, public PHYSICS::cPhysicsRayCast
		{
			cVehicle *pParent;

		public:
			bool bFront; //At the moment only the front wheels are steerable

			float fSuspensionK; //spring constant
			float fSuspensionU; //damping constant
			float fSuspensionNormal;
			float fSuspensionMin;
			float fSuspensionMax;

			float fTraction;

			MATH::cVec3 v3SuspensionTopRel; //Don't use this for rendering
			MATH::cVec3 v3SuspensionTop;

			cWheel(cVehicle *p);

			void Init(bool bFront, float fWRadius, float fInWeight, float fSK, float fSU, 
				float fSNormal, float fSMin, float fSMax, MATH::cVec3 &pos);
			void Update(float fCurrentTime);

			
			// For raycasting to find out if this particular wheel is touching anything
			void RayCast();
			static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);
		};
	}
}

#endif //CVEHICLEWHEEL_H