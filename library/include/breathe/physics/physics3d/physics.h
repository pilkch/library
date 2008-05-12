#ifndef CPHYSICS3D_H
#define CPHYSICS3D_H

#error "Use Physics abstraction layer"
#error "http://www.adrianboeing.com/pal/current/pal-latest.zip"

namespace breathe
{
	namespace physics
	{
		extern dWorldID world;
		extern dSpaceID spaceStatic;
		extern dSpaceID spaceDynamic;
		extern dJointGroupID contactgroup;
		extern dGeomID ground;

		void Init(float posX, float posY, float posZ, float nX, float nY, float nZ);
	}
}

#endif //CPHYSICS3D_H
