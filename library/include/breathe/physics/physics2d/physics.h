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

    b2Body* GetBorder0();
    b2Body* GetBorder1();
    b2Body* GetBorder2();
    b2Body* GetBorder3();

    bool CanSleep();
	}
}

#endif //CPHYSICS2D_H
