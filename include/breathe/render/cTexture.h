#ifndef CTEXTURE_H
#define CTEXTURE_H

// libopenglmm headers
#include <libopenglmm/cTexture.h>

// Spitfire headers
#include <spitfire/util/cSmartPtr.h>

namespace breathe
{
  namespace render
  {
    // ** Texture modes for materials

    enum class TEXTURE_MODE {
      NONE,
      NORMAL,
      MASK,
      BLEND,
      DETAIL,
      SPECULAR,
      CUBE_MAP,
      POST_RENDER
    };

    enum class TEXTURE_TYPE {
      RGBA,
      HEIGHTMAP,
      FRAMEBUFFEROBJECT
    };


    using opengl::cTexture;
    using opengl::cTextureFrameBufferObject;


    typedef cSmartPtr<cTexture> cTextureRef;
    typedef cSmartPtr<cTextureFrameBufferObject> cTextureFrameBufferObjectRef;
  }
}

#endif // CTEXTURE_H
