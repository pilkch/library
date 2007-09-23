#ifndef CPHYSICSRAYCAST_H
#define CPHYSICSRAYCAST_H

namespace breathe
{
	namespace PHYSICS
	{		
		class cPhysicsRayCast
		{
		public:
			dGeomID geomRay;
			cContact rayContact;

			cPhysicsRayCast();
			~cPhysicsRayCast();
			void Create(float fLength);

			virtual void RayCast()=0;
			//static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);
		};
	}
}

#endif //CPHYSICSRAYCAST_H
