#include <cstdlib>
#include <cmath>

#include <vector>
#include <list>
#include <string>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/geometry.h>


#ifdef BUILD_DEBUG

#include <spitfire/util/unittest.h>

class cSphereUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cSphereUnitTest() :
    cUnitTestBase(TEXT("cSphereUnitTest"))
  {
  }

  void Test()
  {
    spitfire::math::cSphere a;
    a.position.Set(3.0f, 1.0f, 1.0f);
    a.SetRadius(1.0f);

    spitfire::math::cSphere b;
    b.position.Set(7.0f, 1.0f, 1.0f);
    b.SetRadius(1.0f);

    float f = 0.0f;

    // Should be approximately 2.0f
    f = a.GetDistance(b);
    ASSERT(spitfire::math::AreApproximatelyEqual(f, 2.0f));

    // Should be approximately 4.0f
    f = a.GetDistanceCentreToCentre(b);
    ASSERT(spitfire::math::AreApproximatelyEqual(f, 4.0f));


    // Should not collide
    a.SetRadius(2.9f);
    ASSERT(!a.Collide(b));

    // Should collide
    a.SetRadius(3.5f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(4.0f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(4.5f);
    ASSERT(a.Collide(b));

    // Should collide
    a.SetRadius(100.0f);
    ASSERT(a.Collide(b));
  }
};

cSphereUnitTest gSphereUnitTest;
#endif // BUILD_DEBUG
