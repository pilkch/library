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

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cTexture.h>

struct SDL_GLContextState;
typedef struct SDL_GLContextState* SDL_GLContext;

#ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
struct SDL_Surface;
#endif

#ifdef __WIN__
#pragma push_macro("CreateFont")
#undef CreateFont
#endif

namespace opengl
{
#ifdef BUILD_LIBOPENGLMM_FONT
  class cFont;
#endif
  class cShader;
  class cStaticVertexBufferObject;

  enum class LIGHT_TYPE {
    POINTLIGHT,
    DIRECTIONAL,
    SPOTLIGHT,
  };

  enum class MODE2D_TYPE {
    Y_INCREASES_UP_SCREEN, // Y increases up the screen (ie. a video game will usually have the origin at the bottom left of the screen)
    Y_INCREASES_UP_SCREEN_KEEP_ASPECT_RATIO, // As above, but using the context aspect ratio instead of 1:1
    Y_INCREASES_UP_SCREEN_KEEP_DIMENSIONS_AND_ASPECT_RATIO, // As above, but using the context width and height
    Y_INCREASES_DOWN_SCREEN, // Y increases down the screen (ie. a GUI will usually have the origin at the top left of the screen)
    Y_INCREASES_DOWN_SCREEN_KEEP_ASPECT_RATIO, // As above, but using the context aspect ratio instead of 1:1
    Y_INCREASES_DOWN_SCREEN_KEEP_DIMENSIONS_AND_ASPECT_RATIO, // As above, but using the context width and height
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
    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    friend class cWindow;
    #endif

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    cContext(cSystem& system, cWindow& window); // Created for a window
    #endif
    cContext(cSystem& system, const cResolution& resolution, bool bIsRenderingToWindow); // Created for an offscreen context or a context for gtkglext
    ~cContext();

    SDL_GLContext GetSDLContext() const { return context; }
    SDL_GLContext GetSDLContext() { return context; }

    bool IsValid() const;

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }

    const spitfire::math::cMat4& GetProjectionMatrix() const { return matProjection; }
    const spitfire::math::cMat4& GetModelViewMatrix() const { return matModelView; }
    const spitfire::math::cMat3& GetTextureMatrix() const { return matTexture; }

    spitfire::math::cMat4 CalculateProjectionMatrix() const;
    spitfire::math::cMat4 CalculateProjectionMatrix(float fFOV) const;
    spitfire::math::cMat4 CalculateProjectionMatrixRenderMode2D(MODE2D_TYPE type) const;

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    void ResizeWindow(cWindow& window, const cResolution& resolution);
    #else
    void ResizeWindow(const cResolution& resolution);
    #endif

    void CreateTexture(cTexture& texture, const opengl::string_t& sFileName);
    void CreateTextureNoMipMaps(cTexture& texture, const opengl::string_t& sFileName);
    void CreateTextureFromImage(cTexture& texture, const voodoo::cImage& image);
    void CreateTextureFromImageNoMipMaps(cTexture& texture, const voodoo::cImage& image);
    void CreateTextureFromBuffer(cTexture& texture, const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void CreateTextureFromBufferNoMipMaps(cTexture& texture, const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTexture(cTexture& texture);

    void CreateTextureCubeMap(
      cTextureCubeMap& texture,
      const opengl::string_t& filePathPositiveX,
      const opengl::string_t& filePathNegativeX,
      const opengl::string_t& filePathPositiveY,
      const opengl::string_t& filePathNegativeY,
      const opengl::string_t& filePathPositiveZ,
      const opengl::string_t& filePathNegativeZ
    );

    void CreateTextureCubeMapFloat(
      cTextureCubeMap& texture,
      const opengl::string_t& filePathPositiveX,
      const opengl::string_t& filePathNegativeX,
      const opengl::string_t& filePathPositiveY,
      const opengl::string_t& filePathNegativeY,
      const opengl::string_t& filePathPositiveZ,
      const opengl::string_t& filePathNegativeZ
    );
    void DestroyTextureCubeMap(cTextureCubeMap& texture);

    void CreateTextureFrameBufferObject(cTextureFrameBufferObject& fbo, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void CreateTextureFrameBufferObject(cTextureFrameBufferObject& fbo, size_t width, size_t height, PIXELFORMAT pixelFormat, const cTextureFrameBufferObject::FLAGS& flags);
    void CreateTextureFrameBufferObjectNoMipMaps(cTextureFrameBufferObject& fbo, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void CreateTextureFrameBufferObjectNoMipMaps(cTextureFrameBufferObject& fbo, size_t width, size_t height, PIXELFORMAT pixelFormat, const cTextureFrameBufferObject::FLAGS& flags);
    void CreateTextureFrameBufferObjectDepthShadowOnlyNoMipMaps(cTextureFrameBufferObject& fbo, size_t width, size_t height);
    void CreateTextureFrameBufferObjectDepthOnly(cTextureFrameBufferObject& fbo, size_t width, size_t height);
    void CreateTextureFrameBufferObjectWithDepth(cTextureFrameBufferObject& fbo, size_t width, size_t height);
    void DestroyTextureFrameBufferObject(cTextureFrameBufferObject& pTexture);

    void CreateShader(cShader& shader, const opengl::string_t& sVertexShaderFileName, const opengl::string_t& sFragmentShaderFileName);
    void CreateShaderFromText(cShader& shader, const std::string& sVertexShaderName, const std::string& sVertexShaderText, const std::string& sFragmentShaderName, const std::string& sFragmentShaderText, const opengl::string_t& sFolderPath, const std::map<std::string, int>& mapDefinesToAdd = std::map<std::string, int>());
    void DestroyShader(cShader& pShader);

    void CreateStaticVertexBufferObject(cStaticVertexBufferObject& vbo);
    void DestroyStaticVertexBufferObject(cStaticVertexBufferObject& vbo);

#ifdef BUILD_LIBOPENGLMM_FONT
    void CreateFont(cFont& font, const opengl::string_t& sFileName, size_t fontSize, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader);
    void DestroyFont(cFont& font);
#endif


    void SetClearColour(const spitfire::math::cColour& clearColour);
    void SetAmbientColour(const spitfire::math::cColour& ambientColour);
    void SetSunPosition(const spitfire::math::cVec3& sunPosition);
    void SetSunAmbientColour(const spitfire::math::cColour& sunAmbientColour);
    void SetSunIntensity(float fSunIntensity);

    void BeginRenderToScreen();
    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    void EndRenderToScreen(cWindow& window);
    #else
    void EndRenderToScreen();
    #endif

    void BeginRenderToTexture(cTextureFrameBufferObject& texture);
    void BeginRenderToCubeMapTexture(cTextureFrameBufferObject& texture);
    void BeginRenderToCubeMapTextureFace(cTextureFrameBufferObject& texture, CUBE_MAP_FACE face);
    void EndRenderToTexture(cTextureFrameBufferObject& texture);

    void BeginRenderMode2D(MODE2D_TYPE type);
    void EndRenderMode2D();


    void EnableCulling();
    void DisableCulling();

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


#ifdef BUILD_LIBOPENGLMM_FONT
    void BindFont(cFont& font);
    void UnBindFont(cFont& font);
#endif


    void BindTexture(size_t uTextureUnit, const cTexture& texture);
    void UnBindTexture(size_t uTextureUnit, const cTexture& texture);

    void BindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture);
    void UnBindTextureCubeMap(size_t uTextureUnit, const cTextureCubeMap& texture);

    void BindTextureCubeMap(size_t uTextureUnit, const cTextureFrameBufferObject& texture); // NOTE: This pair of functions are pretty gross, a cubemap could be a cTextureCubeMap or a cTextureFrameBufferObject so we have to overload here
    void UnBindTextureCubeMap(size_t uTextureUnit, const cTextureFrameBufferObject& texture);

    void BindTextureDepthBuffer(size_t uTextureUnit, const cTextureFrameBufferObject& texture);
    void UnBindTextureDepthBuffer(size_t uTextureUnit, const cTextureFrameBufferObject& texture);

    void BindShader(cShader& shader);
    void UnBindShader(cShader& shader);

    bool SetShaderConstant(const std::string& sConstant, unsigned int value);
    bool SetShaderConstant(const std::string& sConstant, int value);
    bool SetShaderConstant(const std::string& sConstant, float value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec2& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec3& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec4& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour3& value); // NOTE: This will try to look for a vec3 in the shader
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour& value); // NOTE: This will try to look for a vec4 in the shader
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat3& matrix);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cMat4& matrix);

    // A shader must already be bound before these are called
    void SetShaderProjectionAndViewAndModelMatrices(const spitfire::math::cMat4& matProjection, const spitfire::math::cMat4& matView, const spitfire::math::cMat4& matModel);
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

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    bool _SetWindowVideoMode(cWindow& window, bool bIsFullScreen);
    #endif
    void _SetDefaultFlags();
    void _SetPerspective(size_t width, size_t height);

    void _BeginRenderShared(size_t width, size_t height);
    void _EndRenderShared();

    cSystem& system;

    bool bIsRenderingToWindow;
    bool bIsValid;
    cResolution resolution;

    SDL_GLContext context;

    #ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
    SDL_Surface* pSurface;
    #endif

    size_t targetWidth;
    size_t targetHeight;

    spitfire::math::cMat4 matProjection;
    spitfire::math::cMat4 matModelView;
    spitfire::math::cMat3 matTexture;

    float fNear;
    float fFar;

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

#ifdef BUILD_LIBOPENGLMM_FONT
    std::list<cFont*> fonts;
#endif
  };
}

#endif // LIBOPENGLMM_CONTEXT_H
