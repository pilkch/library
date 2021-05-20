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
