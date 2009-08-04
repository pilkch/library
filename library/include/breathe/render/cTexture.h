#ifndef CTEXTURE_H
#define CTEXTURE_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/cSmartPtr.h>

#include <breathe/breathe.h>

struct SDL_Surface;

namespace breathe
{
  namespace render
  {
    // ** Texture modes for materials

    const unsigned int TEXTURE_NONE = 0;
    const unsigned int TEXTURE_NORMAL = 1;
    const unsigned int TEXTURE_MASK = 2;
    const unsigned int TEXTURE_BLEND = 3;
    const unsigned int TEXTURE_DETAIL = 4;
    const unsigned int TEXTURE_CUBEMAP = 5;
    const unsigned int TEXTURE_POST_RENDER = 6;

    const unsigned int TEXTURE_RGBA = 0;
    const unsigned int TEXTURE_HEIGHTMAP = 1;
    const unsigned int TEXTURE_FRAMEBUFFEROBJECT = 2;


    // ** cTexture

    class cTexture
    {
    public:
      cTexture();
      virtual ~cTexture();

      bool IsValid() const { return _IsValid(); }

      size_t GetWidth() const { return uiWidth; }
      size_t GetHeight() const { return uiHeight; }

      bool Load(const string_t& sFilename);

      // pData <--> surface -> OpenGL texture
      void CopyFromDataToSurface();

      void CopyFromSurfaceToData(unsigned int w, unsigned int h);
      void CopyFromSurfaceToData();

      void CopyFromSurfaceToTexture();


      bool SaveToBMP(const string_t& sFilename) const;

      void Transform(float& u, float& v) const;

      void Create() { _Create(); }
      void Destroy();
      void Reload();


      unsigned int uiTextureAtlas;
      unsigned int uiTexture;
      string_t sFilename;

      unsigned int uiWidth;
      unsigned int uiHeight;
      unsigned int uiMode;

      float fScale;
      float fU;
      float fV;

      SDL_Surface* surface;
      std::vector<unsigned char> data;

    private:
      virtual bool _IsValid() const { return (uiTexture != 0); }
      virtual void _Create();
    };

    inline void cTexture::Transform(float& u, float& v) const
    {
      u = (u * fScale) + fU;
      v = (v * fScale) + fV;
    }


    // ** cTextureFrameBufferObject

    class cTextureFrameBufferObject : public cTexture
    {
    public:
      cTextureFrameBufferObject();
      ~cTextureFrameBufferObject();

      unsigned int uiFBO;            // Our handle to the FBO
      unsigned int uiFBODepthBuffer; // Our handle to the depth render buffer

    private:
      bool _IsValid() const { return (uiTexture != 0) && (uiFBO != 0) && (uiFBODepthBuffer != 0); }
      void _Create();
    };

    typedef cSmartPtr<cTexture> cTextureRef;
    typedef cSmartPtr<cTextureFrameBufferObject> cTextureFrameBufferObjectRef;
  }
}

#endif // CTEXTURE_H
