#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/util/unittest.h>
#include <spitfire/math/math.h>


namespace spitfire
{
  namespace math
  {
  }
}

#ifdef BUILD_DEBUG
class cMathUnitsUnitTest : protected breathe::util::cUnitTestBase
{
public:
  cMathUnitsUnitTest() :
    cUnitTestBase(TEXT("cMathUnitsUnitTest"))
  {
    printf("cMathUnitsUnitTest\n");
  }

  void Test();
};


void cMathUnitsUnitTest::Test()
{
   {
      const float fValueOriginal = 12345.67f;

      math::cLength length;
      length.SetFromKiloMeters(fValueOriginal);
      ASSERT(IsApproximatelyEqual(fValueOriginal, length.GetKiloMeters()));

      math::cForce force;
      force.SetFromKiloWatts(fValueOriginal);
      ASSERT(IsApproximatelyEqual(fValueOriginal, force.GetKiloWatts()));

      math::cTorque torque;
      torque.SetFromNewtonMeters(fValueOriginal);
      ASSERT(IsApproximatelyEqual(fValueOriginal, torque.GetNewtonMeters()));

      math::cSpeed speed;
      speed.SetFromMetersPerSecond(fValueOriginal);
      ASSERT(IsApproximatelyEqual(fValueOriginal, speed.GetMetersPerSecond()));
   }

  {
    const float fKW = 150.0f;
    const float fRPM = 4000.0f;

    math::cForce forceOriginal;
    forceOriginal.SetFromKiloWatts(fKW);

    const math::cTorque torque(forceOriginal.GetTorqueAtRPM(fRPM));

    const math::cForce forceConverted(torque.GetForceAtRPM(fRPM));

    const float fResult = forceConverted.GetKiloWatts();
    if (!IsApproximatelyEqual(fKW, fResult)) {
      breathe::stringstream_t o;
      o<<TEXT("cMathUnitsUnitTest FAILED force to torque to force, fKW=");
      o<<fKW;
      o<<TEXT(", fResult=");
      o<<fResult;
      SetFailed(o.str());
    }
  }
}

cMathUnitsUnitTest gMathUnitsUnitTest;
#endif
