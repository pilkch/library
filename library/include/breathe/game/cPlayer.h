#ifndef CPLAYER_H
#define CPLAYER_H

namespace breathe
{
	namespace vehicle
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

	const int CAMERA_FIRSTPERSON = 0;
	const int CAMERA_THIRDPERSON = 1;
#ifdef BUILD_DEBUG
	const int CAMERA_THIRDPERSONFREE = 2;
	const int CAMERA_FIRSTPERSONFREE = 3;
#endif

	class cPlayer : virtual public physics::cUprightCapsule
	{
	public:
		vehicle::cSeat* pSeat;
		
		cPlayer();
		~cPlayer();

		unsigned int uiState;

		unsigned int uiCameraMode;

		float fInputUp;
		float fInputDown;
		float fInputLeft;
		float fInputRight;
		
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
		std::vector<cItem*>vItem;

		void Update(sampletime_t currentTime);

		bool IsInACar() const;
		bool IsDriving() const;
		bool IsThirdPersonCamera() const;

		void ChangeStateToDriving();
		void ChangeStateToRunning();

		void ChangeItemUp();
		void ChangeItemDown();

	
		// For raycasting to find out if we standing on anything when we are in walking mode
		void RayCast();
		static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);
	};
}

#endif //CPLAYER_H
