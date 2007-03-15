#ifndef CITEM_H
#define CITEM_H

namespace BREATHE
{
	class cItem : virtual public PHYSICS::cPhysicsObject
	{
	public:
		/*cLevel *pLevel;
		RENDER::cRender *pRender;
		PHYSICS::cPhysics *pPhysics;*/
    
		cItem();
		~cItem();
	};
}

#endif //CITEM_H
