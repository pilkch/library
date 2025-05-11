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

#ifndef LIBOPENGLMM_CSHADER_H
#define LIBOPENGLMM_CSHADER_H

// Standard headers
#include <map>
#include <set>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cColour.h>

#include <libopenglmm/libopenglmm.h>

namespace opengl
{
  class cContext;

  class cShaderConstants
  {
  public:
    cShaderConstants() {}
    cShaderConstants(const cShaderConstants& rhs) { Assign(rhs); }

    cShaderConstants& operator=(const cShaderConstants& rhs) { Assign(rhs); return *this; }

    int GetValueInt(const std::string& sName) const;
    float GetValueFloat(const std::string& sName) const;
    spitfire::math::cVec2 GetValueVec2(const std::string& sName) const;
    spitfire::math::cVec3 GetValueVec3(const std::string& sName) const;
    spitfire::math::cVec4 GetValueVec4(const std::string& sName) const;

    void SetValue(const std::string& sName, int value) { mInt[sName] = value; }
    void SetValue(const std::string& sName, float value) { mFloat[sName] = value; }
    void SetValue(const std::string& sName, const spitfire::math::cVec2& value) { mVec2[sName] = value; }
    void SetValue(const std::string& sName, const spitfire::math::cVec3& value) { mVec3[sName] = value; }
    void SetValue(const std::string& sName, const spitfire::math::cVec4& value) { mVec4[sName] = value; }

  private:
    void Assign(const cShaderConstants& rhs);

    std::map<std::string, int> mInt;
    std::map<std::string, float> mFloat;
    std::map<std::string, spitfire::math::cVec2> mVec2;
    std::map<std::string, spitfire::math::cVec3> mVec3;
    std::map<std::string, spitfire::math::cVec4> mVec4;
  };

  // void cRender::SetMaterial(material::cMaterialRef pMaterial);
  // void cRender::SetMaterial(material::cMaterialRef pMaterial, const cShaderConstants& shaderConstants);



  // A cShader is a combination of a either one or zero vertex program and one or zero fragment program
  class cShader
  {
  public:
    friend class cContext;

    cShader();

    bool LoadVertexShaderOnly(const opengl::string_t& sShaderVertex);
    bool LoadFragmentShaderOnly(const opengl::string_t& sShaderFragment);
    bool LoadVertexShaderAndFragmentShader(const opengl::string_t& sShaderVertex, const opengl::string_t& sShaderFragment);
    bool LoadVertexShaderAndFragmentShaderFromText(const std::string& sVertexShaderName, const std::string& sVertexShaderText, const std::string& sFragmentShaderName, const std::string& sShaderFragmentText, const opengl::string_t& sFolderPath, const std::map<std::string, int>& mapDefinesToAdd = std::map<std::string, int>());
    void Destroy();

    void Bind();
    void UnBind();

    void CheckStatusVertex();
    void CheckStatusFragment();
    void CheckStatusProgram();

    bool IsCompiledVertex() const;
    bool IsCompiledFragment() const;
    bool IsCompiledProgram() const;

    const string_t GetShaderVertex() const { return sShaderVertex; }
    const string_t GetShaderFragment() const { return sShaderFragment; }

  protected:
    opengl::string_t sShaderVertex;
    opengl::string_t sShaderFragment;

    unsigned int uiShaderVertex;
    unsigned int uiShaderFragment;

    // Combined resource id
    unsigned int uiShaderProgram;


    // Which uniforms should we send to the shader?
    bool bProjectionMatrix;
    bool bViewMatrix;
    bool bViewProjectionMatrix;
    bool bModelMatrix;
    bool bModelViewMatrix;
    bool bModelViewProjectionMatrix;
    bool bNormalMatrix;

    enum class TEXTURE_UNIT_TYPE {
      DISABLED,
      TEXTURE_1D,
      TEXTURE_2D,
      TEXTURE_2D_RECT,
      TEXTURE_2D_SHADOW,
      TEXTURE_CUBE,
    };
    TEXTURE_UNIT_TYPE textureUnitType[MAX_TEXTURE_UNITS];

    // Near and far distances
    bool bNear;
    bool bFar;

    bool bCameraPos;
    bool bAmbientColour;
    bool bSunPosition;
    bool bSunAmbientColour;
    bool bSunIntensity;

    size_t nLights;

  private:
    class cParserContext
    {
    public:
      explicit cParserContext(const string_t& sFolderPath);

      bool AlreadyIncludedFile(const string_t& sFilePath) const;

      opengl::string_t sFolderPath;
      size_t uShaderVersion;

      std::set<opengl::string_t> includedFilePaths;
    };

    void _LoadVertexShader(const opengl::string_t& sShaderVertex);
    void _LoadFragmentShader(const opengl::string_t& sShaderFragment);
    void _LoadVertexShaderFromText(const opengl::string_t& sShaderName, const std::string& sText, const opengl::string_t& sFolderPath, const std::map<std::string, int>& mapDefinesToAdd = std::map<std::string, int>());
    void _LoadFragmentShaderFromText(const opengl::string_t& sShaderName, const std::string& sText, const opengl::string_t& sFolderPath, const std::map<std::string, int>& mapDefinesToAdd = std::map<std::string, int>());
    void _Compile();

    size_t ParseVersion(const std::string& sLine) const;
    std::string ParseInclude(cParserContext& parserContext, const std::string& sLine) const;
    void ParseLineShader(const std::string& sLine);
    void ParseLineFragmentShader(const std::string& sLine);
    void ParseLineVertexShader(const std::string& sLine);

    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    std::vector<std::string> vAttributes;
    std::vector<std::string> vFragmentDataLocations;
    #endif
  };
}

#endif // LIBOPENGLMM_CSHADER_H
