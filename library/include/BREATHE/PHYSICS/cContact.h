#ifndef CPHYSICSCONTACT_H
#define CPHYSICSCONTACT_H

namespace BREATHE
{
	namespace PHYSICS
	{
		class cContact
		{
			dContact contact;

			bool bContact;

		public:
			cContact();

			bool Contact();
			dContact GetContact();

			void SetGeom(dContactGeom o1, dGeomID o2);
			
			void Clear();
			void SetFDSSlipCoefficient1(float fSlip1);
			void SetFDSSlipCoefficient2(float fSlip2);
			void SetCoulombFriction(float fFriction);
			void SetFrictionDirection1(MATH::cVec3 v3Friction);
			void SetElasticity(float fERP, float fCFM);
			void SetBounce(float fBounce, float fBounceVelocity);
			
			void CreateContact(float fContact);
		};
	}
}

#endif //CPHYSICSCONTACT_H
