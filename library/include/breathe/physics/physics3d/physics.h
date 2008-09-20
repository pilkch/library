#ifndef CPHYSICS3D_H
#define CPHYSICS3D_H

namespace breathe
{
  namespace physics
  {
    dSpaceID GetSpaceStatic();
    dSpaceID GetSpaceDynamic();

    dWorldID GetWorld();

    void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ);
    void Init(float width, float height, float depth);
  }
}

#endif // CPHYSICS3D_H
