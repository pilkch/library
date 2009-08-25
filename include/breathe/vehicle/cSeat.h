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
      explicit cSeat(cVehicle* v);
      ~cSeat() { if (pPlayer != nullptr) EjectPlayer(); }

      void AssignPlayer(cPlayer* p);
      void EjectPlayer();

      cPlayer* pPlayer;
      cVehicle* pVehicle;
    };
  }
}

#endif // CVEHICLESEAT_H
