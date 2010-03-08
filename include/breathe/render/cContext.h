#ifndef CCONTEXT_H
#define CCONTEXT_H

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>

#include <breathe/render/camera.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/model/cMesh.h>

// TODO: Remove we should not have to know about anything in cSystem.h
#include <breathe/render/cSystem.h>

namespace breathe
{
  namespace render
  {
    class cWindow;

    // http://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29

    class cContext
    {
    public:
      cContext();
      ~cContext();

      bool IsValid() const;

      bool CreateSharedContextFromWindow(const cWindow& window);
      bool CreateSharedContextFromContext(const cContext& context);
      void Destroy();

    private:
      NO_COPY(cContext);
    };
  }
}

#endif // CCONTEXT_H
