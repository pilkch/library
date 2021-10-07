#include <iostream>
#include <cmath>

#include <vector>
#include <limits>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/math/units.h>

TEST(SpitfireMath, TestUnits)
{
  {
    const float fValueOriginal = 12345.67f;

    spitfire::math::cLength length;
    length.SetFromKiloMeters(fValueOriginal);
    std::cout<<"length="<<length.GetKiloMeters()<<std::endl;
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fValueOriginal, length.GetKiloMeters(), 0.1f));

    spitfire::math::cForce force;
    force.SetFromNewtons(fValueOriginal);
    std::cout<<"newtons="<<force.GetNewtons()<<std::endl;
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fValueOriginal, force.GetNewtons(), 0.1f));

    spitfire::math::cPower power;
    power.SetFromKiloWatts(fValueOriginal);
    std::cout<<"kw="<<power.GetKiloWatts()<<std::endl;
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fValueOriginal, power.GetKiloWatts(), 0.1f));

    spitfire::math::cTorque torque;
    torque.SetFromNewtonMeters(fValueOriginal);
    std::cout<<"nm="<<torque.GetNewtonMeters()<<std::endl;
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fValueOriginal, torque.GetNewtonMeters(), 0.1f));

    spitfire::math::cSpeed speed;
    speed.SetFromMetersPerSecond(fValueOriginal);
    std::cout<<"m/s="<<speed.GetMetersPerSecond()<<std::endl;
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fValueOriginal, speed.GetMetersPerSecond(), 0.1f));
  }

  {
    const float fKW = 150.0f;
    const float fRPM = 4000.0f;

    spitfire::math::cPower powerOriginal;
    powerOriginal.SetFromKiloWatts(fKW);

    const spitfire::math::cTorque torque(powerOriginal.GetTorqueAtRPM(fRPM));

    const spitfire::math::cPower powerConverted(torque.GetPowerAtRPM(fRPM));

    const float fResult = powerConverted.GetKiloWatts();
    ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fKW, fResult));
  }
}

TEST(SpitfireMath, TestUnitsPressure)
{
  {
    // https://en.wikipedia.org/wiki/Adiabatic_process#Example_of_adiabatic_compression
    // P2 = P1(V1/V2)^r
    // P2 = 100000 * (0.001/0.0001)^1.4
    // P2 = 2511886.432
    const float fInitialVolumeLitres = 1.0f;
    const float fInitialVolumeCubicMeters = fInitialVolumeLitres / 1000.0f;
    const float fResultingVolumeLitres = 0.1f;
    const float fResultingVolumeCubicMeters = fResultingVolumeLitres / 1000.0f;
    const float fPressureKPa = 100.0f;
    const float fPressurePa = fPressureKPa * 1000.0f;
    const float fPressure2Pa = spitfire::math::GetPressurePaCompressGasAdiabatically(fInitialVolumeCubicMeters, fPressurePa, fResultingVolumeCubicMeters);
    EXPECT_NEAR(fPressure2Pa, 2511886.432f, 1.0f);

    const float fTemperatureCelcius = 27.0f;
    const float fTemperatureKelvins = spitfire::math::DegreesCelciusToKelvin(fTemperatureCelcius);
    const float fTemperature2Kelvins = spitfire::math::GetTemperatureKelvinsCompressGasAdiabatically(fInitialVolumeCubicMeters, fTemperatureKelvins, fPressurePa, fResultingVolumeCubicMeters, fPressure2Pa);
    const float fTemperature2Celcius = spitfire::math::KelvinToDegreesCelcius(fTemperature2Kelvins);
    EXPECT_NEAR(fTemperature2Celcius, 479.0f, 2.0f);
  }

  {
    // https://phys.libretexts.org/Bookshelves/University_Physics/Book%3A_University_Physics_(OpenStax)/Book%3A_University_Physics_II_-_Thermodynamics_Electricity_and_Magnetism_(OpenStax)/03%3A_The_First_Law_of_Thermodynamics/3.07%3A_Adiabatic_Processes_for_an_Ideal_Gas
    // p2 = (1.00×10^5 N/m2)(240×10^−6 m3 / 40×10^−6 m3) ^ 1/40
    // p2 = 1.23×10^6 N/m2
    const float fInitialVolumeLitres = 0.24f;
    const float fInitialVolumeCubicMeters = fInitialVolumeLitres / 1000.0f;
    const float fResultingVolumeLitres = 0.04f;
    const float fResultingVolumeCubicMeters = fResultingVolumeLitres / 1000.0f;
    const float fPressureKPa = 100.0f; // Approximate sea level atmospheric pressure
    const float fPressurePa = fPressureKPa * 1000.0f;
    const float fPressure2Pa = spitfire::math::GetPressurePaCompressGasAdiabatically(fInitialVolumeCubicMeters, fPressurePa, fResultingVolumeCubicMeters);
    const float fPressure2KPa = fPressure2Pa / 1000.0f;
    EXPECT_NEAR(fPressure2KPa, 1230.0f, 3.0f);

    const float fTemperatureCelcius = 20.0f;
    const float fTemperatureKelvins = spitfire::math::DegreesCelciusToKelvin(fTemperatureCelcius);
    const float fTemperature2Kelvins = spitfire::math::GetTemperatureKelvinsCompressGasAdiabatically(fInitialVolumeCubicMeters, fTemperatureKelvins, fPressurePa, fResultingVolumeCubicMeters, fPressure2Pa);
    const float fTemperature2Celcius = spitfire::math::KelvinToDegreesCelcius(fTemperature2Kelvins);
    EXPECT_NEAR(fTemperature2Celcius, 328.0f, 1.0f);

    const float fRequiredJoules = spitfire::math::GetWorkRequiredJoulesToCompressGasAdiabatically(fInitialVolumeCubicMeters, fTemperatureKelvins, fPressurePa, fResultingVolumeCubicMeters, fPressure2Pa);
    EXPECT_NEAR(fRequiredJoules, -63.0f, 1.0f);
  }

  {
    const float fCompressionRatio = 10.0f / 1.0f;
    const float fInitialVolumeLitres = 1.0f;
    const float fInitialVolumeCubicMeters = fInitialVolumeLitres / 1000.0f;
    const float fResultingVolumeLitres = 1.0f / fCompressionRatio;
    const float fResultingVolumeCubicMeters = fResultingVolumeLitres / 1000.0f;
    const float fTemperatureKelvins = 300.0f; // ~27 °C
    const float fPressureKPa = 100.0f; // Approximate sea level atmospheric pressure
    const float fPressurePa = fPressureKPa * 1000.0f;
    const float fPressure2Pa = spitfire::math::GetPressurePaCompressGasAdiabatically(fInitialVolumeCubicMeters, fPressurePa, fResultingVolumeCubicMeters);
    const float fRequiredJoules = spitfire::math::GetWorkRequiredJoulesToCompressGasAdiabatically(fInitialVolumeCubicMeters, fTemperatureKelvins, fPressurePa, fResultingVolumeCubicMeters, fPressure2Pa);
    EXPECT_NEAR(fRequiredJoules, -378.0f, 1.0f);
  }
}

TEST(SpitfireMath, TestUnitsGetForceOfDragN)
{
  const float fCoefficientOfFriction = 0.30f;
  const float fObjectFrontalAreaMetersSquared = 2.2f;
  const float fDensityOfMediumKgPerMeterSquared = 1.29f;

  {
    const float fSpeedOfObjectMetersPerSecond = 1.0f;
    EXPECT_NEAR(spitfire::math::GetForceOfDragN(fCoefficientOfFriction, fObjectFrontalAreaMetersSquared, fDensityOfMediumKgPerMeterSquared, fSpeedOfObjectMetersPerSecond), 0.4257f, 0.1f);
  }

  {
    const float fSpeedOfObjectMetersPerSecond = 10.0f;
    EXPECT_NEAR(spitfire::math::GetForceOfDragN(fCoefficientOfFriction, fObjectFrontalAreaMetersSquared, fDensityOfMediumKgPerMeterSquared, fSpeedOfObjectMetersPerSecond), 42.57f, 0.1f);
  }
}

TEST(SpitfireMath, TestUnitsGearRatios)
{
  const float fGearRatio = 3.0f / 1.0f;
  const float fInputRPM = 2700.0f;
  const float fInputTorqueNm = 140.0f;

  const float fOutputRPM = spitfire::math::GetGearOutputRPM(fGearRatio, fInputRPM);
  EXPECT_NEAR(fOutputRPM, 900.0f, 0.1f);

  const float fOutputTorqueNm = spitfire::math::GetGearOutputTorqueNm(fGearRatio, fInputTorqueNm);
  EXPECT_NEAR(fOutputTorqueNm, 420.0f, 0.1f);
}
