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

    enum class TEXTURE_MODE {
      TEXTURE_NONE,
      TEXTURE_NORMAL,
      TEXTURE_MASK,
      TEXTURE_BLEND,
      TEXTURE_DETAIL,
      TEXTURE_SPECULAR,
      TEXTURE_CUBE_MAP,
      TEXTURE_POST_RENDER
    };

    enum class TEXTURE_TYPE {
      TEXTURE_RGBA,
      TEXTURE_HEIGHTMAP,
      TEXTURE_FRAMEBUFFEROBJECT
    };


    // ** cTexture

    class cTexture
    {
    public:
      cTexture();
      virtual ~cTexture();

      bool IsValid() const { return _IsValid(); }

      size_t GetWidth() const { return uiWidth; }
      size_t GetHeight() const { return uiHeight; }

      void SetWidth(size_t _uiWidth) { uiWidth = _uiWidth; }
      void SetHeight(size_t _uiHeight) { uiHeight = _uiHeight; }

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

    protected:
      unsigned int uiWidth;
      unsigned int uiHeight;

    public:
      TEXTURE_TYPE uiType;
      TEXTURE_MODE uiMode;

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

    const size_t DEFAULT_FBO_TEXTURE_WIDTH = 1024;
    const size_t DEFAULT_FBO_TEXTURE_HEIGHT = 1024;

    class cTextureFrameBufferObject : public cTexture
    {
    public:
      cTextureFrameBufferObject();
      ~cTextureFrameBufferObject();

      bool IsModeCubeMap() const { return (uiMode == TEXTURE_MODE::TEXTURE_CUBE_MAP); }
      void SetModeCubeMap();

      void GenerateMipMapsIfRequired();

      void SelectMipMapLevelOfDetail(float fLevelOfDetail);

      unsigned int uiFBO;            // Our handle to the FBO
      unsigned int uiFBODepthBuffer; // Our handle to the depth render buffer

    private:
      bool _IsValid() const { return (uiTexture != 0) && (uiFBO != 0) && (uiFBODepthBuffer != 0); }
      void _Create();

      bool bIsUsingMipMaps;
    };


    typedef cSmartPtr<cTexture> cTextureRef;
    typedef cSmartPtr<cTextureFrameBufferObject> cTextureFrameBufferObjectRef;
  }
}

#endif // CTEXTURE_H
