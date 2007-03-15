#ifndef CPLAYER_H
#define CPLAYER_H

namespace BREATHE
{
	namespace VEHICLE
	{
		class cSeat;
	}

	class cItem;

	const int PLAYER_DEAD=0;
	const int PLAYER_WALKING=1;
	const int PLAYER_DRIVING=2;

	class cPlayer : virtual public PHYSICS::cPhysicsObject
	{
	public:
		VEHICLE::cSeat *pSeat;
		
		cPlayer();
		~cPlayer();

		bool bAccelerate;
		bool bBrake;
		bool bLeft;
		bool bRight;
		bool bHandbrake;
		bool bClutch;

		bool bState;

		float fDollars;

		//list of items
		std::vector<cItem *>vItem;

		void Init(MATH::cVec3 &pos);
		void Update(float fTime);

		bool isDriving()
		{
			return NULL==pSeat;
		}
	};
}

#endif //CPLAYER_H
