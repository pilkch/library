#include <iostream>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire Includes
#include <spitfire/math/units.h>


#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cMathUnitsUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cMathUnitsUnitTest() :
    cUnitTestBase(TEXT("cMathUnitsUnitTest"))
  {
  }

  virtual void Test() override;
};


void cMathUnitsUnitTest::Test()
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

cMathUnitsUnitTest gMathUnitsUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
