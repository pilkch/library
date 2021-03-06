#ifndef CUNITS_H
#define CUNITS_H

#include <spitfire/math/math.h>

namespace spitfire
{
  namespace math
  {
    // http://en.wikipedia.org/wiki/SI_derived_unit
    // http://www.racer.nl/tech/converting.htm

    inline float kphTomph(float kph) { return kph * 1.609344f; }
    inline float mphTokph(float mph) { return mph * 0.621371192f; }

    // 1 international knot = 1 nautical mile per hour (exactly) = 1.852 kilometres per hour (exactly)
    inline float kphToknots(float kph) { return kph * 0.539956803f; }
    inline float knotsTokph(float knots) { return knots * 1.852f; }

    inline float MPGToLitresPerHundredKilometres(float fMPG) { return 235.214583f / fMPG; }
    inline float LitresPerHundredKilometresToMPG(float fLitresPerHundredKilometres) { return 235.214583f / fLitresPerHundredKilometres; }


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

    // kW = Nm * (2 * pi * RPM / 60000);
    // Nm = kW / (2 * pi * RPM / 60000);

    const float c1_OVER_RPM_KW = 1.0f / 60000.0f;

    inline float NmToKw(float Nm, float RPM) { return Nm * (c2_PI * RPM * c1_OVER_RPM_KW); }
    inline float KwToNm(float kW, float RPM) { return kW / (c2_PI * RPM * c1_OVER_RPM_KW); }

    inline float RPMToRadiansPerSecond(float RPM) { return RPM * 0.10471976f; }
    inline float RadiansPerSecondToRPM(float RadiansPerSecond) { return RadiansPerSecond * 9.5493f; }

    inline float m_s_to_km_h(float m_s) { return m_s * 3.6f; }
    inline float km_h_to_m_s(float km_h) { return km_h / 3.6f; }



    // http://en.wikipedia.org/wiki/Kelvin
    inline float KelvinToDegreesCelcius(float fKelvin) { return fKelvin - 273.15f; }
    inline float DegreesCelciusToKelvin(float fDegreesCelcius) { return fDegreesCelcius + 273.15f; }


    // http://en.wikipedia.org/wiki/Pounds_per_square_inch
    // ^ = square
    // 1 pascal (Pa) ≡ 1 N/(m^2) ≡ 1 J/(m^3) ≡ 1 kg/(m*(s^2))
    inline float PaTopsi(float Pa) { return Pa * 6894.75728001037f; }
    inline float psiToPa(float psi) { return psi * 0.000145037681f; }

    inline float KPaTopsi(float KPa) { return KPa * 6.89475728001037f; }
    inline float psiToKPa(float psi) { return psi * 0.145037738007f; }

    // http://en.wikipedia.org/wiki/Cubic_metre_per_second
    inline float CubicMetresPerSecondToCubicFeetPerSecond(float fCubicMetresPerSecond) { return fCubicMetresPerSecond * 35.314454f; }
    inline float CubicFeetPerSecondToCubicMetresPerSecond(float fCubicFeetPerSecond) { return fCubicFeetPerSecond * 0.0283170172f; }


    // http://en.wikipedia.org/wiki/Density_of_air
    // http://en.wikipedia.org/wiki/ISO_1
    const float cAMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER = 1.2041f; // 1.2 kg/m3
    const float cAMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA = 101.325f;
    const float cAMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS = 20.0f;


    // http://en.wikipedia.org/wiki/Density_of_air#Altitude
    // returns density of air in kg/m3 for the given altitude in meters
    inline float GetDensityOfAirKgPerCubicMeterForAltitudeMeters(float fAltitudeMeters)
    {
      const float h = fAltitudeMeters;
      const float T0 = 288.15f; // sea level standard temperature T0 = 288.15 K
      const float L = 0.0065f; // temperature lapse rate L = 0.0065 K/m
      const float T = T0 - (L * h);

      const float p0 = 101325.0f; // sea level standard atmospheric pressure p0 = 101325 Pa
      const float g = 9.80665f; // Earth-surface gravitational acceleration g = 9.80665 m/s2.
      const float M = 0.0289644f; // molar mass of dry air M = 0.0289644 kg/mol
      const float R = 8.31447f; // universal gas constant R = 8.31447 J/(mol·K)
      const float p = p0 * (pow(1.0f - ((L * h) / T0), (g * M) / (R * L)));

      return (p * M) / (R * T);
    }

    // http://en.wikipedia.org/wiki/Density_of_air#Temperature_and_pressure
    inline float GetDensityOfAirKgPerCubicMeterForPressureKPAAndTemperatureDegreesCelcius(float fPressureKPA, float fTemperatureDegreesCelcius)
    {
      const float p = 1000.0f * fPressureKPA; // absolute pressure (must be in Pa not KPA)
      const float R = 287.05f; // specific gas constant for dry air is 287.05 J/(kg·K) in SI units
      const float T = DegreesCelciusToKelvin(fTemperatureDegreesCelcius); // absolute temperature

      return (p / (R * T));
    }


    inline float CentimetersToMillimeters(float fCentimeters) { return 0.1f * fCentimeters; }
    inline float MillimetersToCentimeters(float fMillimeters) { return 10.0f * fMillimeters; }

    inline float GetCubicCentimetersDisplacementForOneCylinder(float fBoreMillimeters, float fStrokeMillimeters) { return (cPI / 4.0f) * square(MillimetersToCentimeters(fBoreMillimeters)) * MillimetersToCentimeters(fStrokeMillimeters); }

    inline float CubicCentimetersToCubicInches(float fCubicCentimeters) { return 0.0610237441f * fCubicCentimeters; }
    inline float CubicInchesToCubicCentimeters(float fCubicInches) { return 16.387064f * fCubicInches; }


    inline float CentimetersToInches(float fCentimeters) { return fCentimeters * 0.393700787401575f; }
    inline float InchesToCentimeters(float fInches) { return fInches * 2.54f; }
    inline float MetersToFeet(float fMeters) { return fMeters * 3.2808399f; }
    inline float MetersToMiles(float fMeters) { return fMeters * 0.000621371192f; }
    inline float CubicInchesToCubicCentiMeters(float fCubicInches) { return fCubicInches * 16.387064f; }
    inline float CubicCentiMetersToCubicInches(float fCubicCentiMeters) { return fCubicCentiMeters * 0.0610237441f; }
    inline float KiloGramsToPounds(float fKg) { return fKg * 2.20462262f; }
    inline float PoundsToKiloGrams(float fLb) { return fLb * 0.45359237f; }
    inline float KiloMetersPerHourToMilesPerHour(float fKPH) { return kphTomph(fKPH); }
 
    // 1 nautical mile (exactly) = 1.852 kilometres (exactly)
    inline float KilometersToNauticalMiles(float Kilometers) { return Kilometers * 0.539956803f; }
    inline float NauticalMilesToKilometers(float NauticalMiles) { return NauticalMiles * 1.852f; }



    class cLength
    {
    public:
      cLength() : fValueMeters(0.0f) {}

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
      cForce() : fValueNewtons(0.0f) {}

      float GetGrams() const { return fValueNewtons * 1000.0f; }
      float GetKiloGrams() const { return fValueNewtons; }
      float GetTons() const { return fValueNewtons * 0.001f; }
      float GetNewtons() const { return fValueNewtons; }

      float GetPounds() const { return KiloGramsToPounds(fValueNewtons); }


      void SetFromKiloGrams(float fKg) { fValueNewtons = fKg; }
      void SetFromNewtons(float fNetwons) { fValueNewtons = fNetwons; }

    private:
      float fValueNewtons;
    };

    class cPower
    {
    public:
      cPower() : fValueKiloWatts(0.0f) {}

      float GetWatts() const { return fValueKiloWatts * 1000.0f; }
      float GetKiloWatts() const { return fValueKiloWatts; }
      float GetHorsePower() const { return KwToHP(fValueKiloWatts); }


      void SetFromWatts(float fWatts) { fValueKiloWatts = fWatts / 1000.0f; }
      void SetFromKiloWatts(float fKiloWatts) { fValueKiloWatts = fKiloWatts; }


      cTorque GetTorqueAtRPM(float fRPM) const;

    private:
      float fValueKiloWatts;
    };

    class cTorque
    {
    public:
      cTorque() : fValueNm(0.0f) {}

      float GetNewtonMeters() const { return fValueNm; }

      float GetFootPounds() const { return NmTolbft(fValueNm); }

      void SetFromNewtonMeters(float fNm) { fValueNm = fNm; }


      cPower GetPowerAtRPM(float fRPM) const;

    private:
      float fValueNm;
    };

    class cSpeed
    {
    public:
      cSpeed() : fValueKPH(0.0f) {}

      float GetMetersPerSecond() const { return km_h_to_m_s(fValueKPH); }
      float GetKiloMetersPerHour() const { return fValueKPH; }

      float GetMilesPerHour() const { return KiloMetersPerHourToMilesPerHour(fValueKPH); }


      void SetFromMetersPerSecond(float fMetersPerSecond) { fValueKPH = m_s_to_km_h(fMetersPerSecond); }
      void SetFromKiloMetersPerHour(float fKiloMetersPerHour) { fValueKPH = fKiloMetersPerHour; }

    private:
      float fValueKPH;
    };



    // *** Inlines

    inline cTorque cPower::GetTorqueAtRPM(float fRPM) const
    {
      cTorque torque;
      torque.SetFromNewtonMeters(KwToNm(fValueKiloWatts, fRPM));
      return torque;
    }

    inline cPower cTorque::GetPowerAtRPM(float fRPM) const
    {
      cPower power;
      power.SetFromKiloWatts(NmToKw(fValueNm, fRPM));
      return power;
    }
  }
}

#endif // CUNITS_H
