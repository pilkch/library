#ifndef CPLAYER_H
#define CPLAYER_H

namespace BREATHE
{
	namespace VEHICLE
	{
		class cSeat;
	}

	class cItem;

	const int PLAYER_STATE_DEAD=0;
	const int PLAYER_STATE_WALK=1;
	const int PLAYER_STATE_RUN=2;
	const int PLAYER_STATE_SPRINT=3;
	const int PLAYER_STATE_DRIVE=4;
	const int PLAYER_STATE_PASSENGER=5;

	class cPlayer : virtual public PHYSICS::cPhysicsObject
	{
	public:
		VEHICLE::cSeat *pSeat;
		
		cPlayer();
		~cPlayer();

		unsigned int uiState;

		bool bThirdPerson;
#ifdef BUILD_DEBUG
		bool bThirdPersonDebug;
#endif BUILD_DEBUG

		bool bInputUp;
		bool bInputDown;
		bool bInputLeft;
		bool bInputRight;
		
		bool bInputHandbrake;
		bool bInputClutch;

		bool bInputJump;
		bool bInputCrouch;


		float fSpeedWalk;
		float fSpeedRun;
		float fSpeedSprint;

		float fVertical, fHorizontal;

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
