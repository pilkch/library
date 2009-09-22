#ifndef CVEHICLESEAT_H
#define CVEHICLESEAT_H

namespace breathe
{
  class c3ds;

  namespace vehicle
  {
    class cVehicle2;

    class cSeat
    {
    public:
      explicit cSeat(cVehicle2* v);
      ~cSeat() { if (pPlayer != nullptr) EjectPlayer(); }

      void AssignPlayer(cPlayer* p);
      void EjectPlayer();

      cPlayer* pPlayer;
      cVehicle2* pVehicle;
    };
  }
}

#endif // CVEHICLESEAT_H
