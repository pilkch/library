#ifndef PHYSICS_CUPRIGHTCAPSULE_H
#define PHYSICS_CUPRIGHTCAPSULE_H

namespace BREATHE
{
	namespace PHYSICS
	{
		class cPhysics;

		class cUprightCapsule : virtual public cPhysicsObject
		{
		public:
			cUprightCapsule();
			~cUprightCapsule();
			
			void Update(float fTime);
		};
	}
}

#endif //PHYSICS_CUPRIGHTCAPSULE_H
