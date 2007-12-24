#ifndef CPHYSICS2D_H
#define CPHYSICS2D_H

namespace breathe
{
	namespace physics
	{
		void Init(float width, float height);

    float GetWidth();
    float GetHeight();

    b2World* GetWorld();

    cPhysicsObject* GetBorder0();
    cPhysicsObject* GetBorder1();
    cPhysicsObject* GetBorder2();
    cPhysicsObject* GetBorder3();

    bool CanSleep();
	}
}

#endif //CPHYSICS2D_H
