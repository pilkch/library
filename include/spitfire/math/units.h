#ifndef CUNITS_H
#define CUNITS_H

#include <spitfire/math/math.h>

namespace spitfire
{
  namespace math
  {
    // http://en.wikipedia.org/wiki/SI_derived_unit
    // http://www.racer.nl/tech/converting.htm

    inline float m_s_to_km_h(float m_s) { return m_s * 3.6f; }
    inline float km_h_to_m_s(float km_h) { return km_h / 3.6f; }

    inline float kphTomph(float kph) { return kph * 1.609344f; }
    inline float mphTokph(float mph) { return mph * 0.621371192f; }

    // 1 international knot = 1 nautical mile per hour (exactly) = 1.852 kilometres per hour (exactly)
    inline float kphToknots(float kph) { return kph * 0.539956803f; }
    inline float knotsTokph(float knots) { return knots * 1.852f; }

    inline float MPGToLitresPerHundredKilometres(float fMPG) { return 235.214583f / fMPG; }
    inline float LitresPerHundredKilometresToMPG(float fLitresPerHundredKilometres) { return 235.214583f / fLitresPerHundredKilometres; }



    // Electricity
    //
    // I - Current, amps
    // V - Voltage, volts
    // R - Resistance, ohms
    // P - Power, watts


    // Ohm’s Law
    // The current through the conductor is directly proportional to applied voltage and is expressed as:
    //
    // I = V / R
    //
    // where I – current, measured in amperes (A);
    // V – applied voltage, measured in volts (V);
    // R – resistance, measured in ohms (Ω).
    //
    constexpr float GetCurrentAmpsThroughConductor(float fAppliedVoltage, float fResistanceOhms)
    {
      const float fCurrentAmps = fAppliedVoltage / fResistanceOhms;
      return fCurrentAmps;
    }


    // The consumed electrical power of the motor is defined by the following formula:
    //
    // Pin = I * V
    //
    // where Pin – input power, measured in watts (W);
    // I – current, measured in amperes (A);
    // V – applied voltage, measured in volts (V).
    //
    constexpr float GetInputConsumedPowerWatts(float fCurrentAmps, float fAppliedVoltage)
    {
      const float fInputPowerWatts = fCurrentAmps * fAppliedVoltage;
      return fInputPowerWatts;
    }




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


    // Motors
    // https://simplemotor.com/calculations/

    // The output mechanical power of the motor can be calculated by using the following formula:
    //
    // Pout = τ * ω
    //
    // where Pout – output power, measured in watts (W);
    // τ – torque, measured in Newton meters (N•m);
    // ω – angular speed, measured in radians per second (rad/s).
    //
    constexpr float GetMotorOutputMechanicalPowerW(float fTorqueNM, float fAngularSpeedRadiansPerSecond)
    {
      const float fOutputPowerWatts = fTorqueNM * fAngularSpeedRadiansPerSecond;
      return fOutputPowerWatts;
    }


    // Calculate angular speed if you know rotational speed of the motor in rpm:
    //
    // ω = rpm * 2π / 60
    //
    // where ω – angular speed, measured in radians per second (rad/s);
    // rpm – rotational speed in revolutions per minute;
    // π – mathematical constant pi (3.14).
    // 60 – number of seconds in a minute.
    //
    constexpr float GetMotorAngularSpeedRadiansPerSecond(float fRPM)
    {
      const float fOutputMotorAngularSpeedRadiansPerSecond = fRPM * ((2.0f * spitfire::math::cPI) / 60.0f);
      return fOutputMotorAngularSpeedRadiansPerSecond;
    }


    // Gearing up and down

    constexpr float GetGearOutputRPM(float fGearRatio, float fInputRPM)
    {
      if (fGearRatio == 0.0f) return 0.0f;

      return fInputRPM / fGearRatio;
    }

    constexpr float GetGearOutputTorqueNm(float fGearRatio, float fInputTorqueNm)
    {
      return fGearRatio * fInputTorqueNm;
    }



    // Mass and force

    constexpr float MassKgToForceNewtons(float fMassKg)
    {
      return fMassKg * 9.8066500286389f;
    }

    constexpr float ForceNewtonsToMassKg(float fForceNewtons)
    {
      return fForceNewtons * 0.101971621f;
    }


    // Inertia
    //
    // https://courses.lumenlearning.com/physics/chapter/10-3-dynamics-of-rotational-motion-rotational-inertia/


    // NOTE: The axis goes through the "circles", a drive shaft for example

    constexpr float CalculateMomentOfInertiaCylinder(float fRadiusMeters, float fMassKg)
    {
      return (fMassKg * (fRadiusMeters * fRadiusMeters)) / 2.0f;
    }


    // NOTE: The axis goes through the ring, a tire for example

    constexpr float CalculateMomentOfInertiaRing(float fInnerRadiusMeters, float fOuterRadiusMeters, float fMassKg)
    {
      return (fMassKg * ((fInnerRadiusMeters * fInnerRadiusMeters) + (fOuterRadiusMeters * fOuterRadiusMeters))) / 2.0f;
    }


    constexpr float TorqueFromMomentOfInertiaAndAngularlAcceleration(float fMomentOfInertia, float fAngularAccelerationRadiansPerSecondSquared)
    {
      // t = Ia
      return fMomentOfInertia * fAngularAccelerationRadiansPerSecondSquared;
    }

    constexpr float AngularlAccelerationRadiansPerSecondSquaredFromMomentOfInertiaAndTorque(float fMomentOfInertia, float fTorqueNm)
    {
      // a = t / I
      return fTorqueNm / fMomentOfInertia;
    }


    // NOTE: A wheel is roughly a a cylinder for the rim plus a ring for the tire








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


    // https://en.wikipedia.org/wiki/Density_of_air
    // At 20 °C and 101.325 kPa, dry air has a density of 1.2041 kg/m³
    // http://en.wikipedia.org/wiki/ISO_1
    const float GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER = 1.2041f; // 1.2 kg/m3
    const float GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA = 101.325f; // kPa
    const float GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS = 20.0f; // °C


    // http://en.wikipedia.org/wiki/Density_of_air#Altitude
    // returns density of air in kg/m3 for the given altitude in meters
    // NOTE: This assumes the temperature and pressure of the air falls off at a known rate towards the atmosphere, ignoring heat from the sun, local humidity, and local wind changes
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


    
    // Get the temperature, density, and pressure for an altitude in the standard atmosphere
    // Correct to 86 km.  Only approximate thereafter.
    // NOTE: This is just an approximation ignoring the current weather effects:
    // 1. Heat from the sun
    // 2. Local humidity
    // 3. Local wind changes
    // https://www.pdas.com/atmosdownload.html
    inline void GetApproximateAtmosphereAtAltitudeMeters(
        float fAltitudeMeters, // geometric altitude, meters
        float& fDensityKgPerCubicMeter, // Density/sea-level standard density
        float& fPressurePa,   // Pressure/sea-level standard pressure
        float& fTemperatureCelcius,   // Temperature/sea-level standard temperature
        float& fSpeedOfSoundMetersPerSecond
    ) {
      fDensityKgPerCubicMeter = 0.0f;
      fPressurePa = 0.0f;
      fTemperatureCelcius = 0.0f;
      fSpeedOfSoundMetersPerSecond = 0.0f;

      const double REARTH = 6356.766;    // polar radius of the Earth (km)
      const double GMR = 34.163195;
      const size_t NTAB = 8;

      static double htab[NTAB] = { 0.0,  11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 84.852 };
      static double ttab[NTAB] = { 288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65, 186.946 };
      static double ptab[NTAB] = { 1.0, 2.2336110E-1, 5.4032950E-2, 8.5666784E-3, 1.0945601E-3, 6.6063531E-4, 3.9046834E-5, 3.68501E-6 };
      static double gtab[NTAB] = { -6.5, 0, 1.0, 2.8, 0, -2.8, -2.0, 0.0 };

      const float fAltitudeKm = fAltitudeMeters / 1000.0f;
      const double fAltitudeGeoPotentialKm = fAltitudeKm * REARTH / (fAltitudeKm + REARTH);     //  geometric to geopotential altitude

      // The original code used a binary search, but that is pointless when we typically want lower altitudes, just check the height of each layer in order
      // NOTE: The altitude for each layer specifies the lower bound of the layer, so we want the highest possible layer that doesn't go over our specified height
      size_t match = 0;
      for (size_t i = 0; i < NTAB; i++) {

        if (fAltitudeGeoPotentialKm <= htab[i]) {
          break;
        }

        match = i;
      }

      double tgrad=gtab[match];                      // temp. gradient of local layer
      double tbase=ttab[match];                      // base temp. of local layer
      double deltah = fAltitudeGeoPotentialKm - htab[match];                   // height above local base
      double tlocal = tbase + tgrad * deltah;          // local temperature

      const double theta = tlocal / ttab[0]; // Temperature/sea-level standard temperature
      double delta = 0.0; // Pressure/sea-level standard pressure

      if (0.0 == tgrad) {                                         // pressure ratio
        delta = ptab[match] * exp(-GMR * deltah / tbase);
      } else {
        delta = ptab[match] * pow(tbase / tlocal, GMR / tgrad);
      }

      const double sigma = delta / theta; // Density/sea-level standard density

      const double TZERO = 288.15; // sea level temperature, kelvins
      const double PZERO = 101325.0; // sea-level pressure, Pa
      const double RHOZERO = 1.225; // sea level density, kg/cu.m
      const double AZERO = 340.294; // sea-level speed of sound, m/sec

      const float fTemperatureKelvins = TZERO * theta;

      fTemperatureCelcius = KelvinToDegreesCelcius(fTemperatureKelvins);
      fPressurePa = PZERO * delta;
      fDensityKgPerCubicMeter = RHOZERO * sigma;
      fSpeedOfSoundMetersPerSecond = AZERO * sqrt(theta);
    }

    // http://en.wikipedia.org/wiki/Density_of_air#Temperature_and_pressure
    inline float GetDensityOfAirKgPerCubicMeterForPressurePAAndTemperatureDegreesCelcius(float fPressurePa, float fTemperatureDegreesCelcius)
    {
      const float p = fPressurePa; // absolute pressure (must be in Pa not KPA)
      const float R = 287.05f; // specific gas constant for dry air is 287.05 J/(kg·K) in SI units
      const float T = DegreesCelciusToKelvin(fTemperatureDegreesCelcius); // absolute temperature

      return (p / (R * T));
    }

    // Adiabatic compression
    // https://phys.libretexts.org/Bookshelves/University_Physics/Book%3A_University_Physics_(OpenStax)/Book%3A_University_Physics_II_-_Thermodynamics_Electricity_and_Magnetism_(OpenStax)/03%3A_The_First_Law_of_Thermodynamics/3.07%3A_Adiabatic_Processes_for_an_Ideal_Gas
    // Assumptions:
    // The gas is air consisting of molecular nitrogen and oxygen only (thus a diatomic gas with 5 degrees of freedom, and so γ = 7/5, or γ = 1.4)
    // Assume the compression happens quickly enough that no heat escapes or transfers out of the volume to the container
    inline float GetPressurePaCompressGasAdiabatically(float fVolumeLitres, float fPressurePa, float fCompressedVolumeLitres)
    {
      const float r = 1.4; // γ = 1.4
      return fPressurePa * pow((fVolumeLitres / fCompressedVolumeLitres), r);
    }

    // Ideal gas law
    inline float GetTemperatureKelvinsCompressGasAdiabatically(float fVolumeLitres, float fTemperatureKelvins, float fPressurePa, float fCompressedVolumeLitres, float fPressure2Pa)
    {
      return ((fPressure2Pa * fCompressedVolumeLitres) / (fPressurePa * fVolumeLitres)) * fTemperatureKelvins;
    }

    inline float GetWorkRequiredJoulesToCompressGasAdiabatically(float fVolumeLitres, float fTemperatureKelvins, float fPressurePa, float fCompressedVolumeLitres, float fPressure2Pa)
    {
      const float r = 1.4; // γ = 1.4
      return (1.0f / (1.0f - r)) * ((fPressure2Pa * fCompressedVolumeLitres) - (fPressurePa * fVolumeLitres));
    }



    // Drag

    // Calculate the force of drag for an object moving through a medium at a specific speed
    //
    // Fdrag = 0.5 * Cd * A * rho * v²
    //
    constexpr float GetForceOfDragN(float fCoefficientOfFriction, float fObjectFrontalAreaMetersSquared, float fDensityOfMediumKgPerMeterSquared, float fSpeedOfObjectMetersPerSecond)
    {
      return 0.5f * fCoefficientOfFriction * fObjectFrontalAreaMetersSquared * fDensityOfMediumKgPerMeterSquared * (fSpeedOfObjectMetersPerSecond * fSpeedOfObjectMetersPerSecond);
    }

    // Calculate the force of lift/downforce for an object moving through a medium at a specific speed
    // NOTE: This is exactly the same equation as we used to calculate the drag force above, but using the coefficient of lift/downforce instead of the coefficient of drag
    // NOTE: When the coefficient of lift/downforce is positive it is generating lift, negative it is generating downforce
    //
    // Fdown = 0.5 * Cl * A * rho * v²
    //
    constexpr float GetForceOfLiftDownforceN(float fCoefficientOfLiftOrDownforce, float fObjectFrontalAreaMetersSquared, float fDensityOfMediumKgPerMeterSquared, float fSpeedOfObjectMetersPerSecond)
    {
      return 0.5f * fCoefficientOfLiftOrDownforce * fObjectFrontalAreaMetersSquared * fDensityOfMediumKgPerMeterSquared * (fSpeedOfObjectMetersPerSecond * fSpeedOfObjectMetersPerSecond);
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
