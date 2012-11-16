/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington      *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

// This is a simple wrapper around OpenGL to make it a little bit more modern and easier to work with

#ifndef LIBOPENGLMM_CONTEXT_H
#define LIBOPENGLMM_CONTEXT_H

// Standard headers
#include <list>
#include <map>

// Spifire headers
#include <spitfire/math/cQuaternion.h>

// liblibopenglmm headers
#include <libopenglmm/libopenglmm.h>

struct SDL_Surface;

namespace opengl
{
#ifdef BUILD_OPENGLMM_FONT
  class cFont;
#endif
  class cTexture;
  class cTextureCubeMap;
  class cTextureFrameBufferObject;
  class cShader;
  class cStaticVertexBufferObject;

  enum class LIGHT_TYPE {
    POINTLIGHT,
    DIRECTIONAL,
    SPOTLIGHT,
  };

  enum class MODE2D_TYPE {
    Y_INCREASES_UP_SCREEN, // Y increases up the screen (ie. a video game will usually have the origin at the bottom left of the screen)
    Y_INCREASES_DOWN_SCREEN // Y increases down the screen (ie. a GUI will usually have the origin at the top left of the screen)
  };

  // OpenGL 3.2 Contexts in SDL
  // http://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_%28C_/_SDL%29

  // OpenGL Multiple Contexts
  // http://www.stevestreeting.com/2006/10/20/gl-thread-taming/
  // 1. Create main rendering thread context
  // 2. Disable main rendering thread context
  // 3. Lock background init condition mutex
  // 4. Start background thread
  // 5. Main thread waits for init condition (this releases the init mutex and blocks the main thread)
  // 6. Background thread clones context, sets up resource sharing and enables its own context
  // 7. Background thread locks the init mutex, notifies parent, releases init mutex, then continues independently
  // 8. Main thread wakes up, re-enables its own context, and carries on too

  class cContext
  {
  public:
    friend class cWindow;

    cContext(cSystem& system, const cWindow& window); // Created for a window
    cContext(cSystem& system, const cResolution& resolution); // Created for an offscreen context
    ~cContext();

    bool IsValid() const;

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }

    const spitfire::math::cMat4& GetProjectionMatrix() const { return matProjection; }
    const spitfire::math::cMat4& GetModelViewMatrix() const { return matModelView; }
    const spitfire::math::cMat3& GetTextureMatrix() const { return matTexture; }

    spitfire::math::cMat4 CalculateProjectionMatrix() const;
    spitfire::math::cMat4 CalculateProjectionMatrixRenderMode2D(MODE2D_TYPE type) const;

    void ResizeWindow(const cResolution& resolution);

    cTexture* CreateTexture(const opengl::string_t& sFileName);
    cTexture* CreateTextureNoMipMaps(const opengl::string_t& sFileName);
    cTexture* CreateTextureFromImage(const voodoo::cImage& image);
    cTexture* CreateTextureFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTexture(cTexture* pTexture);

    cTextureCubeMap* CreateTextureCubeMap(
      const opengl::string_t& filePathPositiveX,
      const opengl::string_t& filePathNegativeX,
      const opengl::string_t& filePathPositiveY,
      const opengl::string_t& filePathNegativeY,
      const opengl::string_t& filePathPositiveZ,
      const opengl::string_t& filePathNegativeZ
    );
    void DestroyTextureCubeMap(cTextureCubeMap* pTexture);

    cTextureFrameBufferObject* CreateTextureFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat);
    cTextureFrameBufferObject* CreateTextureFrameBufferObjectNoMipMaps(size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTextureFrameBufferObject(cTextureFrameBufferObject* pTexture);

    cShader* CreateShader(const opengl::string_t& sVertexShaderFileName, const opengl::string_t& sFragmentShaderFileName);
    void DestroyShader(cShader* pShader);

    cStaticVertexBufferObject* CreateStaticVertexBufferObject();
    void DestroyStaticVertexBufferObject(cStaticVertexBufferObject* pStaticVertexBufferObject);

#ifdef BUILD_OPENGLMM_FONT
    cFont* CreateFont(const opengl::string_t& sFileName, size_t fontSize, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader);
    void DestroyFont(cFont* pFont);
#endif


    void SetClearColour(const spitfire::math::cColour& clearColour);
    void SetAmbientColour(const spitfire::math::cColour& ambientColour);
    void SetSunPosition(const spitfire::math::cVec3& sunPosition);
    void SetSunAmbientColour(const spitfire::math::cColour& sunAmbientColour);
    void SetSunIntensity(float fSunIntensity);

    void BeginRenderToScreen();
    void EndRenderToScreen();

    void BeginRenderToTexture(cTextureFrameBufferObject& texture);
    void EndRenderToTexture(cTextureFrameBufferObject& texture);

    //void BeginRenderToCubeMapTextureFace(cTextureFrameBufferObject& texture, CUBE_MAP_FACE face);
    //void EndRenderToCubeMapTextureFace(cTextureFrameBufferObject& texture);

    void BeginRenderMode2D(MODE2D_TYPE type);
    void EndRenderMode2D();


    void EnableWireframe();
    void DisableWireframe();

    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    void EnableLighting();
    void DisableLighting();
    #endif

    void EnableDepthTesting();
    void DisableDepthTesting();
    void EnableDepthMasking();
    void DisableDepthMasking();
    void EnableAlphaTesting();
    void DisableAlphaTesting();
    void EnableBlending();
    void DisableBlending();


#ifdef BUILD_OPENGLMM_FONT
    void BindFont(const cFont& font);
    void UnBindFont(const cFont& font);
#endif


    void BindTexture(size_t uTextureUnit, const cTexture& texture);
    void UnBindTexture(size_t uTextureUnit, const cTexture& texture);

    void BindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture);
    void UnBindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture);

    void BindShader(cShader& shader);
    void UnBindShader(cShader& shader);

    bool SetShaderConstant(const std::string& sConstant, int value);
    bool SetShaderConstant(const std::string& sConstant, float value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec2& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec3& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec4& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour& value); // NOTE: This will try to look for a vec4 in the shader
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat3& matrix);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat4& matrix);

    // A shader must already be bound before these are called
    void SetShaderProjectionAndModelViewMatrices(const spitfire::math::cMat4& matProjection, const spitfire::math::cMat4& matModelView);
    void SetShaderProjectionAndModelViewMatricesRenderMode2D(MODE2D_TYPE type, const spitfire::math::cMat4& matModelView);
    //void SetTextureMatrix(const spitfire::math::cMat3& matTexture);

    void SetShaderLightEnabled(size_t light, bool bEnabled);
    void SetShaderLightType(size_t light, LIGHT_TYPE type);
    void SetShaderLightPosition(size_t light, const spitfire::math::cVec3& position);
    void SetShaderLightRotation(size_t light, const spitfire::math::cQuaternion& rotation);
    void SetShaderLightAmbientColour(size_t light, const spitfire::math::cColour& colour);
    void SetShaderLightDiffuseColour(size_t light, const spitfire::math::cColour& colour);
    void SetShaderLightSpecularColour(size_t light, const spitfire::math::cColour& colour);


    // cStaticVertexBufferObject

    void BindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject);
    void UnBindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject);
    void BindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject);
    void UnBindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject);

    void DrawStaticVertexBufferObjectLines(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectTriangles(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectTriangleStrip(cStaticVertexBufferObject& staticVertexBufferObject);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Quads are deprecated in OpenGL 3.1 core profile
    void DrawStaticVertexBufferObjectQuads(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectQuadStrip(cStaticVertexBufferObject& staticVertexBufferObject);
    #endif

    void DrawStaticVertexBufferObjectLines2D(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectTriangles2D(cStaticVertexBufferObject& staticVertexBufferObject);
    #ifndef BUILD_LIBOPENGLMM_OPENGL_STRICT
    // Quads are deprecated in OpenGL 3.1 core profile
    void DrawStaticVertexBufferObjectQuads2D(cStaticVertexBufferObject& staticVertexBufferObject);
    #endif

  private:
    spitfire::math::cMat4 CalculateProjectionMatrix(size_t width, size_t height, float fFOV) const;

    bool _SetWindowVideoMode(bool bIsFullScreen);
    void _SetDefaultFlags();
    void _SetPerspective(size_t width, size_t height);

    void _BeginRenderShared(size_t width, size_t height);
    void _EndRenderShared();

    cSystem& system;

    bool bIsRenderingToWindow;
    bool bIsValid;
    cResolution resolution;

    SDL_Surface* pSurface;

    spitfire::math::cMat4 matProjection;
    spitfire::math::cMat4 matModelView;
    spitfire::math::cMat3 matTexture;

    spitfire::math::cColour clearColour;
    spitfire::math::cColour ambientColour;

    spitfire::math::cVec3 sunPosition;
    spitfire::math::cColour sunAmbientColour;
    float fSunIntensity;

    cShader* pCurrentShader;

    std::map<opengl::string_t, cTexture*> textures;
    std::map<opengl::string_t, cTextureCubeMap*> cubeMapTextures;
    std::list<cShader*> shaders;
    std::list<cStaticVertexBufferObject*> staticVertexBufferObjects;

#ifdef BUILD_OPENGLMM_FONT
    std::list<cFont*> fonts;
#endif
  };
}

#endif // LIBOPENGLMM_CONTEXT_H
