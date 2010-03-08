#ifndef CWINDOW_H
#define CWINDOW_H

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>

#include <breathe/render/camera.h>
#include <breathe/render/cContext.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/model/cMesh.h>

namespace breathe
{
  namespace render
  {
    class cWindow
    {
    public:
      cWindow();
      ~cWindow();

      bool Create();
      void Destroy();

    private:
      NO_COPY(cWindow);

      cContext context;
    };
  }
}

#endif // CWINDOW_H
