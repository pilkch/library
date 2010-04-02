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

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cColour.h>

//#include <libopenglmm/cTexture.h>
//#include <libopenglmm/cShader.h>
//#include <libopenglmm/cVertexBufferObject.h>
//#include <libopenglmm/cWindow.h>

namespace opengl
{
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
    cShader();

    bool LoadVertexShaderOnly(const std::string& sShaderVertex);
    bool LoadFragmentShaderOnly(const std::string& sShaderFragment);
    bool LoadVertexShaderAndFragmentShader(const std::string& sShaderVertex, const std::string& sShaderFragment);
    void Destroy();

    void CheckStatusVertex();
    void CheckStatusFragment();
    void CheckStatusProgram();

    bool IsCompiledVertex() const;
    bool IsCompiledFragment() const;
    bool IsCompiledProgram() const;

    std::string sShaderVertex;
    std::string sShaderFragment;

    unsigned int uiShaderVertex;
    unsigned int uiShaderFragment;

    // Combined resource id
    unsigned int uiShaderProgram;


    // Which uniforms should we send to the shader?
    bool bTexUnit0;
    bool bTexUnit1;
    bool bTexUnit2;
    bool bTexUnit3;
    bool bCameraPos;
    bool bAmbientColour;
    bool bLightPosition;

  private:
    void _LoadVertexShader(const std::string& sShaderVertex);
    void _LoadFragmentShader(const std::string& sShaderFragment);
    void _Compile();
  };
}

#endif // LIBOPENGLMM_CSHADER_H
