#ifndef CUNITS_H
#define CUNITS_H

#include <spitfire/math/math.h>

namespace spitfire
{
  namespace math
  {
    // http://www.racer.nl/tech/converting.htm
    // TODO: Consider using Boost.Units library
    // http://www.boost.org/doc/libs/1_38_0/doc/html/boost_units/FAQ.html#boost_units.FAQ.NoConstructorFromValueType
    //typedef boost::quantity<boost::length> cLength;
    //typedef boost::quantity<boost::force> cForce;
    //typedef boost::quantity<boost::torque> cTorque;
    //typedef boost::quantity<boost::volume> cVolume;
    //typedef boost::quantity<boost::area> cArea;
    //typedef boost::quantity<boost::time> cTime;


    inline float kphTomph(float kph) { return kph * 1.609344f; }
    inline float mphTokph(float mph) { return mph * 0.621371192f; }

    // 1 international knot = 1 nautical mile per hour (exactly) = 1.852 kilometres per hour (exactly)
    inline float kphToknots(float kph) { return kph * 0.539956803f; }
    inline float knotsTokph(float knots) { return knots * 1.852f; }

    // Power/Torque
    // kW/Nm : hp/lb-ft

    inline float HPToKw(float hp) { return hp * 0.74569987158227022f; }
    inline float KwToHP(float kW) { return kW * 1.34102209f; }

    inline float NmTolbft(float Nm) { return Nm * 1.3558179483314004f; }
    inline float lbftToNm(float lbft) { return lbft * 0.7375621f; }

    // wheel torque = torque at the engine * gear ratio
    // speed = rpm * gear ratio * circumference of the wheels

    // power = torque * angular_speed
    // angular_speed = 2 * pi * rotational_speed
    // rotational_speed = revolutions per unit time.

    // power (in kW) = torque (in Nm) * 2 * pi * rotational speed (in rpm) / 60000.0f
    // where 60000.0f comes from 60 seconds per minute times 1000.0f watts per kilowatt.

    // kW = Nm * 2 * pi * RPM / 60000;
    // Nm = kW / (2 * pi * RPM / 60000);

    const float c1_OVER_RPM_KW = 1.0f / 60000.0f;

    inline float NmToKw(float Nm, float RPM) { return Nm * c2_PI * RPM * c1_OVER_RPM_KW; }
    inline float KwToNm(float kW, float RPM) { return kW / (c2_PI * RPM * c1_OVER_RPM_KW); }

    inline float RPMToRadiansPerSecond(float RPM) { return RPM * 0.10471976f; }
    inline float RadiansPerSecondToRPM(float RadiansPerSecond) { return RadiansPerSecond * 9.5493f; }

    inline float m_s_to_km_h(float m_s) { return m_s * 3.6f; }
    inline float km_h_to_m_s(float km_h) { return km_h / 3.6f; }

    // ^ = square
    // 1 pascal (Pa) ≡ 1 N/(m^2) ≡ 1 J/(m^3) ≡ 1 kg/(m*(s^2))
    inline float PaTopsi(float Pa) { return Pa * 6894.75728001037f; }
    inline float psiToPa(float psi) { return psi * 0.000145037681f; }

    inline float KPaTopsi(float KPa) { return KPa * 6.89475728001037f; }
    inline float psiToKPa(float psi) { return psi * 0.145037738007f; }






    inline float MetersToFeet(float fMeters) { return fMeters * 3.2808399f; }
    inline float MetersToMiles(float fMeters) { return fMeters * 0.000621371192f; }
    //inline float KiloGramsToWatts(float fKg) { return fKg * 0.0025f; }
    //inline float KiloGramsToKiloWatts(float fKg) { return fKg * 0.0025f; }
    inline float KiloGramsToPounds(float fKg) { return fKg * 2.20462262f; }
    //inline float KiloGramsToHorsePower(float fKg) { return fKg * 1.0f; }
    //inline float HorsePowerToKiloGrams(float fHP) { return fHP * 1.0f; }
    //inline float KiloWattsToKiloGrams(float fKw) { return fKw * 0.0025f; }
    inline float KiloMetersPerHourToMilesPerHour(float fKPH) { return kphTomph(fKPH); }

    // 1 nautical mile (exactly) = 1.852 kilometres (exactly)
    inline float KilometersToNauticalMiles(float Kilometers) { return Kilometers * 0.539956803f; }
    inline float NauticalMilesToKilometers(float NauticalMiles) { return NauticalMiles * 1.852f; }



    class cLength
    {
    public:
      float GetCentiMeters() const { return fValueMeters * 100.0f; }
      float GetMeters() const { return fValueMeters; }
      float GetKiloMeters() const { return fValueMeters * 0.001f; }

      float GetFeet() const { return MetersToFeet(fValueMeters); }
      float GetMiles() const { return MetersToMiles(fValueMeters); }


      void SetFromCentiMeters(float fCm) { fValueMeters = fCm * 0.01f; }
      void SetFromMeters(float fMeters) { fValueMeters = fMeters; }
      void SetFromKiloMeters(float fKiloMeters) { fValueMeters = fKiloMeters * 1000.0f; }

    private:
      float fValueMeters;
    };

    class cTorque;

    class cForce
    {
    public:
      float GetGrams() const { return fValueKg * 1000.0f; }
      float GetKiloGrams() const { return fValueKg; }
      float GetTons() const { return fValueKg * 0.001f; }

      //float GetWatts() const { return KiloGramsToWatts(fValueKg); }
      //float GetKiloWatts() const { return KiloGramsToKiloWatts(fValueKg); }

      float GetPounds() const { return KiloGramsToPounds(fValueKg); }

      float GetHorsePower() const { return KiloGramsToPounds(fValueKg); }


      void SetFromKiloGrams(float fKg) { fValueKg = fKg; }
      //void SetFromKiloWatts(float fKW) { fValueKg = KiloWattsToKiloGrams(fKW); }

      //void SetFromHorsepower(float fHP) { fValueKg = HorsePowerToKiloGrams(fHP); }


      cTorque GetTorqueAtRPM(float fRPM) const;

    private:
      float fValueKg;
    };

    class cTorque
    {
    public:
      float GetNewtonMeters() const { return fValueNm; }

      //float GetFootPounds() const { return fValueNm * ; }

      void SetFromNewtonMeters(float fNm) { fValueNm = fNm; }


      cForce GetForceAtRPM(float fRPM) const;

    private:
      float fValueNm;
    };

    class cSpeed
    {
    public:
      float GetMetersPerSecond() const { return km_h_to_m_s(fValueKPH); }
      float GetKiloMetersPerHour() const { return fValueKPH; }

      float GetMilesPerHour() const { return KiloMetersPerHourToMilesPerHour(fValueKPH); }


      void SetFromMetersPerSecond(float fMetersPerSecond) { fValueKPH = m_s_to_km_h(fMetersPerSecond); }
      void SetFromKiloMetersPerHour(float fKiloMetersPerHour) { fValueKPH = fKiloMetersPerHour; }

    private:
      float fValueKPH;
    };



    // *** Inlines

    inline cTorque cForce::GetTorqueAtRPM(float fRPM) const
    {
      cTorque f;
      ASSERT(false);
      //f.SetFromNewtonMeters(KwToNm(GetKiloWatts(), fRPM));
      return f;
    }

    inline cForce cTorque::GetForceAtRPM(float fRPM) const
    {
      cForce f;
      ASSERT(false);
      //f.SetFromKiloWatts(NmToKw(GetNewtonMeters(), fRPM));
      return f;
    }
  }
}

#endif // CUNITS_H
