#ifndef CPHYSICS3D_H
#define CPHYSICS3D_H

namespace breathe
{
	namespace physics
	{
		extern dWorldID world;
		extern dSpaceID spaceStatic;
		extern dSpaceID spaceDynamic;
		extern dJointGroupID contactgroup;
		extern dGeomID ground;

		void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ);
	}
}

#endif //CPHYSICS3D_H
