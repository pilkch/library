#ifndef CPHYSICSOBJECT_H
#define CPHYSICSOBJECT_H

namespace breathe
{
	namespace physics
	{
		// *****************************************************************************************************
		// Upright Capsule, use this for characters that cannot fall over
		// Basically the same as a normal capsule, just with constraints that: 
		// a) Stop the capsule tipping over
		// b) Help the capsule up stairs by pushing the capsule up when we collide a ray in front of our feet
		// *****************************************************************************************************
		class cUprightCapsule : virtual public cPhysicsObject, public cPhysicsRayCast
		{
		public:
			cUprightCapsule();

			void Update(sampletime_t currentTime);
		};
	}
}

#endif //CPHYSICSOBJECT_H
