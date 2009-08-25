#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>
#include <string>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/loader_3ds/build3ds.h>
#include <breathe/loader_3ds/camera3ds.h>

namespace breathe
{
  namespace loader_3ds
  {
    const float fScale=0.1f;

    Camera3DS::Camera3DS()
    {
    }

    Camera3DS::~Camera3DS()
    {
    }

    void Camera3DS::Parse(Model3DSChunk c)
    {
      LOG.Success("3ds", "Camera");

      math::cFrustum *p=new math::cFrustum();

      p->eye.x=p->eyeIdeal.x= fScale * c.Float();
      p->eye.y=p->eyeIdeal.y= fScale * c.Float();
      p->eye.z=p->eyeIdeal.z= fScale * c.Float();

      p->target.x=p->targetIdeal.x= fScale * c.Float();
      p->target.y=p->targetIdeal.y= fScale * c.Float();
      p->target.z=p->targetIdeal.z= fScale * c.Float();

      //float bank_angle= fScale * c.Float();
      //float focus= fScale * c.Float();

      //vCamera.push_back(p);
    }
  }
}
