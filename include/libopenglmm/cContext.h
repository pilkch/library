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
  class cTexture;
  class cShader;
  class cVertexBufferObject;

  class cContext
  {
  public:
    friend class cWindow;

    cContext(cSystem& system, const cWindow& window); // Created for a window
    cContext(cSystem& system, const cResolution& resolution); // Created for an offscreen context
    ~cContext();

    bool IsValid() const { return false; }

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }


    cTexture* CreateTexture(const std::string& sFileName);
    cTexture* CreateTexture(size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTexture(cTexture* pTexture);

    cShader* CreateShader(const std::string& sVertexShaderFileName, const std::string& sFragmentShaderFileName);
    void DestroyShader(cShader* pShader);

    cVertexBufferObject* CreateVertexBufferObject();
    void DestroyVertexBufferObject(cVertexBufferObject* pVertexBufferObject);


    void BeginRendering() {}
    void EndRendering() {}


    void BindTexture(size_t uTextureUnit, const cTexture& texture) {}
    void UnBindTexture(size_t uTextureUnit, const cTexture& texture) {}


    void BindShader(const cShader& shader) {}
    void UnBindShader(const cShader& shader) {}


    const spitfire::math::cMat4& GetProjectionMatrix() const { return matProjection; }
    const spitfire::math::cMat4& GetModelViewMatrix() const { return matModelView; }
    const spitfire::math::cMat4& GetTextureMatrix() const { return matTexture; }

    // A shader must already be bound before these are called
    void SetProjectionMatrix(const spitfire::math::cMat4& matrix);
    void SetModelViewMatrix(const spitfire::math::cMat4& matrix);
    void SetTextureMatrix(const spitfire::math::cMat4& matrix);


    void BindVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}
    void UnBindVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}

    void DrawVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}

  protected:
    void ReloadResources() {}

  private:
    cSystem& system;
    bool bIsValid;
    cResolution resolution;

    SDL_Surface* pSurface;

    // matProjection and matModelView are multiplied together to get the MPV matrix in OpenGL 3.0
    spitfire::math::cMat4 matProjection;
    spitfire::math::cMat4 matModelView;
    spitfire::math::cMat4 matTexture;

    std::map<std::string, cTexture*> textures;
    std::vector<cShader*> shaders;
    std::vector<cVertexBufferObject*> vertexBufferObjects;
  };
}

#endif // LIBOPENGLMM_CONTEXT_H
