#ifndef CVEHICLESEAT_H
#define CVEHICLESEAT_H

namespace breathe
{
	class c3ds;

	namespace vehicle
	{
		class cVehicle;

		class cSeat
		{
		public:
			cPlayer *pPlayer;
			cVehicle *pVehicle;

			cSeat(cVehicle *v);
			~cSeat();

			void AssignPlayer(cPlayer *p);
			void EjectPlayer();
		};
	}
}

#endif //CVEHICLESEAT_H
