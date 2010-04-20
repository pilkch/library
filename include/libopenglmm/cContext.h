/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington         *
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

#include <libopenglmm/libopenglmm.h>

struct SDL_Surface;

namespace opengl
{
#ifdef BUILD_OPENGLMM_FONT
  class cFont;
#endif
  class cTexture;
  class cTextureFrameBufferObject;
  class cShader;
  class cStaticVertexBufferObject;

  enum class LIGHT_TYPE {
    POINTLIGHT,
    DIRECTIONAL,
    SPOTLIGHT,
  };

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

    void ResizeWindow(const cResolution& resolution);

    cTexture* CreateTexture(const std::string& sFileName);
    cTexture* CreateTextureFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTexture(cTexture* pTexture);

    cTextureFrameBufferObject* CreateTextureFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTextureFrameBufferObject(cTextureFrameBufferObject* pTexture);

    cShader* CreateShader(const std::string& sVertexShaderFileName, const std::string& sFragmentShaderFileName);
    void DestroyShader(cShader* pShader);

    cStaticVertexBufferObject* CreateStaticVertexBufferObject();
    void DestroyStaticVertexBufferObject(cStaticVertexBufferObject* pStaticVertexBufferObject);

#ifdef BUILD_OPENGLMM_FONT
    cFont* CreateFont(const std::string& sFileName, size_t fontSize);
    void DestroyFont(cFont* pFont);
#endif


    void SetClearColour(const spitfire::math::cColour& clearColour);
    void SetAmbientLightColour(const spitfire::math::cColour& ambientLightColour);

    void BeginRendering();
    void EndRendering();

    void BeginRenderToTexture(cTextureFrameBufferObject& texture);
    void EndRenderToTexture(cTextureFrameBufferObject& texture);

    //void BeginRenderToCubeMapTextureFace(cTextureFrameBufferObject& texture, CUBE_MAP_FACE face);
    //void EndRenderToCubeMapTextureFace(cTextureFrameBufferObject& texture);

    void BeginRenderMode2D();
    void EndRenderMode2D();


    void EnableWireframe();
    void DisableWireframe();


    void EnableLighting();
    void DisableLighting();
    void EnableLight(size_t light);
    void DisableLighting(size_t light);
    void SetLightType(size_t light, LIGHT_TYPE type);
    void SetLightPosition(size_t light, const spitfire::math::cVec3& position);
    void SetLightRotation(size_t light, const spitfire::math::cQuaternion& rotation);
    void SetLightAmbientColour(size_t light, const spitfire::math::cColour& colour);
    void SetLightDiffuseColour(size_t light, const spitfire::math::cColour& colour);
    void SetLightSpecularColour(size_t light, const spitfire::math::cColour& colour);

    // TODO: Remove this, it is primarily for openglmm_gears and can probably be replaced with a shader
    void SetMaterialAmbientColour(const spitfire::math::cColour& colour);
    void SetMaterialDiffuseColour(const spitfire::math::cColour& colour);
    void SetMaterialSpecularColour(const spitfire::math::cColour& colour);
    void SetMaterialShininess(float fShininess);


#ifdef BUILD_OPENGLMM_FONT
    void BindFont(const cFont& font);
    void UnBindFont(const cFont& font);
#endif


    void BindTexture(size_t uTextureUnit, const cTexture& texture);
    void UnBindTexture(size_t uTextureUnit, const cTexture& texture);


    void BindShader(cShader& shader);
    void UnBindShader(cShader& shader);

    bool SetShaderConstant(const std::string& sConstant, int value);
    bool SetShaderConstant(const std::string& sConstant, float value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec2& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec3& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec4& value);
    bool SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour& value); // NOTE: This will try to look for a vec4 in the shader


    const spitfire::math::cMat4& GetProjectionMatrix() const { return matProjection; }
    const spitfire::math::cMat4& GetModelViewMatrix() const { return matModelView; }
    const spitfire::math::cMat4& GetTextureMatrix() const { return matTexture; }

    // A shader must already be bound before these are called
    void SetProjectionMatrix(const spitfire::math::cMat4& matrix);
    void SetModelViewMatrix(const spitfire::math::cMat4& matrix);
    void SetTextureMatrix(const spitfire::math::cMat4& matrix);


    void BindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject);
    void UnBindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject);
    void BindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject);
    void UnBindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject);

    void DrawStaticVertexBufferObjectLines(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectTriangles(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectTriangleStrip(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectQuads(cStaticVertexBufferObject& staticVertexBufferObject);
    void DrawStaticVertexBufferObjectQuadStrip(cStaticVertexBufferObject& staticVertexBufferObject);

    void DrawStaticVertexBufferObjectQuads2D(cStaticVertexBufferObject& staticVertexBufferObject);

  protected:
    void ReloadResources() {}

  private:
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

    // matProjection and matModelView are multiplied together to get the MPV matrix in OpenGL 3.0
    spitfire::math::cMat4 matProjection;
    spitfire::math::cMat4 matModelView;
    spitfire::math::cMat4 matTexture;

    spitfire::math::cColour clearColour;
    spitfire::math::cColour ambientLightColour;

    cShader* pCurrentShader;

    std::map<std::string, cTexture*> textures;
    std::vector<cShader*> shaders;
    std::vector<cStaticVertexBufferObject*> staticVertexBufferObjects;
  };
}

#endif // LIBOPENGLMM_CONTEXT_H
