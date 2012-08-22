// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/opengl.h>

bool StringBeginsWith(const std::string& source, const std::string& find)
{
  if (source.length() < find.length()) return false;

  return (source.substr(0, find.length()) == find);
}

std::string StringReplace(const std::string& source, const std::string& sFind, const std::string& sReplace)
{
  size_t j;
  std::string temp(source);
  for (;(j = temp.find(sFind)) != std::string::npos;) {
    temp.replace(j, sFind.length(), sReplace);
  }
  return temp;
}

namespace opengl
{
  // *** cShaderConstants

  void cShaderConstants::Assign(const cShaderConstants& rhs)
  {
    mInt = rhs.mInt;
    mFloat = rhs.mFloat;
    mVec2 = rhs.mVec2;
    mVec3 = rhs.mVec3;
    mVec4 = rhs.mVec4;
  }

  int cShaderConstants::GetValueInt(const std::string& sName) const
  {
    std::map<std::string, int>::const_iterator iter(mInt.find(sName));
    assert(iter != mInt.end());

    return iter->second;
  }

  float cShaderConstants::GetValueFloat(const std::string& sName) const
  {
    std::map<std::string, float>::const_iterator iter(mFloat.find(sName));
    assert(iter != mFloat.end());

    return iter->second;
  }

  spitfire::math::cVec2 cShaderConstants::GetValueVec2(const std::string& sName) const
  {
    std::map<std::string, spitfire::math::cVec2>::const_iterator iter(mVec2.find(sName));
    assert(iter != mVec2.end());

    return iter->second;
  }

  spitfire::math::cVec3 cShaderConstants::GetValueVec3(const std::string& sName) const
  {
    std::map<std::string, spitfire::math::cVec3>::const_iterator iter(mVec3.find(sName));
    assert(iter != mVec3.end());

    return iter->second;
  }

  spitfire::math::cVec4 cShaderConstants::GetValueVec4(const std::string& sName) const
  {
    std::map<std::string, spitfire::math::cVec4>::const_iterator iter(mVec4.find(sName));
    assert(iter != mVec4.end());

    return iter->second;
  }




  // *** cShader

  cShader::cShader() :
    uiShaderVertex(0),
    uiShaderFragment(0),
    uiShaderProgram(0),

    bProjectionMatrix(false),
    bModelViewMatrix(false),
    bModelViewProjectionMatrix(false),
    bNormalMatrix(false),

    bTexUnit0(false),
    bTexUnit1(false),
    bTexUnit2(false),
    bTexUnit3(false),

    bCameraPos(false),
    bAmbientColour(false),
    bSunPosition(false),
    bSunAmbientColour(false),
    bSunIntensity(false),

    nLights(0)
  {
  }

  void cShader::CheckStatusVertex()
  {
    std::cout<<"cShader::CheckStatusVertex Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
    std::cout<<"cShader::CheckStatusVertex glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetShaderInfoLog(uiShaderVertex, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusVertex "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      )
      {
        sInfo = StringReplace(sInfo, "\n", "<br>");
        std::cout<<"cShader::CheckStatusVertex Vertex Shader "<<opengl::string::ToUTF8(sShaderVertex)<<": "<<sInfo<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    std::cout<<"cShader::CheckStatusVertex returning"<<std::endl;
  }

  void cShader::CheckStatusFragment()
  {
    std::cout<<"cShader::CheckStatusFragment Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetShaderiv(uiShaderFragment, GL_INFO_LOG_LENGTH, &infologLength);
    std::cout<<"cShader::CheckStatusFragment glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetShaderInfoLog(uiShaderFragment, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusFragment "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      )
      {
        sInfo = StringReplace(sInfo, "\n", "<br>");
        std::cout<<"cShader::CheckStatusFragment  Fragment Shader "<<opengl::string::ToUTF8(sShaderFragment)<<": "<<sInfo<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    std::cout<<"cShader::CheckStatusFragment returning"<<std::endl;
  }

  void cShader::CheckStatusProgram()
  {
    std::cout<<"cShader::CheckStatusProgram Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetProgramiv(uiShaderProgram, GL_INFO_LOG_LENGTH, &infologLength);
    std::cout<<"cShader::CheckStatusProgram glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetProgramInfoLog(uiShaderProgram, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusProgram "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("Warning") != std::string::npos
      ) {
        std::cout<<"cShader::CheckStatusProgram Program "<<opengl::string::ToUTF8(sShaderVertex)<<" "<<opengl::string::ToUTF8(sShaderFragment)<<": "<<infoLog<<std::endl;
      } else {
        std::cout<<"cShader::CheckStatusProgram Program "<<opengl::string::ToUTF8(sShaderVertex)<<" "<<opengl::string::ToUTF8(sShaderFragment)<<": "<<infoLog<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    std::cout<<"cShader::CheckStatusProgram returning"<<std::endl;
  }

  bool cShader::IsCompiledVertex() const
  {
    GLint value = GL_FALSE;
    glGetShaderiv(uiShaderVertex, GL_COMPILE_STATUS, &value);
    return (value == GL_TRUE);
  }

  bool cShader::IsCompiledFragment() const
  {
    GLint value = GL_FALSE;
    glGetShaderiv(uiShaderFragment, GL_COMPILE_STATUS, &value);
    return (value == GL_TRUE);
  }

  bool cShader::IsCompiledProgram() const
  {
    GLint value = GL_FALSE;
    glGetProgramiv(uiShaderProgram, GL_LINK_STATUS, &value);
    return (value == GL_TRUE);
  }

  void cShader::ParseShaderLine(const std::string& sLine)
  {
    // Warn about deprecated OpenGL 2 built in variables
    if (sLine.find("gl_ModelViewProjectionMatrix") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_ModelViewProjectionMatrix\" should be replaced with \"uniform mat4 matModelViewProjection;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_NormalMatrix") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_NormalMatrix\" should be replaced with \"uniform mat3 matNormal;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.ambient") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_FrontMaterial.ambient\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.diffuse") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_FrontMaterial.diffuse\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.specular") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_FrontMaterial.specular\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.shininess") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_FrontMaterial.shininess\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_LightModel.ambient") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_LightModel.ambient\" should be replaced with \"uniform vec4 ambientColour;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_LightSource") != std::string::npos) {
      std::cerr<<"cShader::ParseShaderLine \"gl_LightSource\" should be replaced with uniforms on the shader"<<std::endl;
      assert(false);
    }

    // Check which uniforms this shader uses
    if (StringBeginsWith(sLine, "uniform mat4 matProjection;")) bProjectionMatrix = true;
    else if (StringBeginsWith(sLine, "uniform mat4 matModelView;")) bModelViewMatrix = true;
    else if (StringBeginsWith(sLine, "uniform mat4 matModelViewProjection;")) bModelViewProjectionMatrix = true;
    else if (StringBeginsWith(sLine, "uniform mat3 matNormal;")) bNormalMatrix = true;
    else if (StringBeginsWith(sLine, "uniform sampler2D texUnit0;")) bTexUnit0 = true;
    else if (StringBeginsWith(sLine, "uniform sampler2D texUnit1;")) bTexUnit1 = true;
    else if (StringBeginsWith(sLine, "uniform sampler2D texUnit2;")) bTexUnit2 = true;
    else if (StringBeginsWith(sLine, "uniform sampler2D texUnit3;")) bTexUnit3 = true;
    else if (StringBeginsWith(sLine, "uniform vec4 ambientColour;")) bAmbientColour = true;
    else if (StringBeginsWith(sLine, "uniform vec3 sunPosition;")) bSunPosition = true;
    else if (StringBeginsWith(sLine, "uniform vec4 sunAmbientColour;")) bSunAmbientColour = true;
    else if (StringBeginsWith(sLine, "uniform float fSunIntensity;")) bSunIntensity = true;
  }

  void cShader::_LoadVertexShader(const opengl::string_t& _sShaderVertex)
  {
    sShaderVertex = _sShaderVertex;

    std::ifstream f(opengl::string::ToUTF8(sShaderVertex).c_str());
    if (f.is_open()) {
      std::ostringstream o;
      std::string sLine;
      while (!f.eof()) {
        std::getline(f, sLine);

        o<<sLine;
        o<<"\n";

        ParseShaderLine(sLine);
      };

      std::cout<<"cShader::_LoadVertexShader Vertex "<<cSystem::GetErrorString()<<" shader=\""<<o.str()<<"\""<<std::endl;

      uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);
      std::cout<<"cShader::_LoadVertexShader Vertex shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusVertex();
      assert(uiShaderVertex != 0);

      const std::string sBuffer = o.str();
      const char* str = sBuffer.c_str();
      glShaderSource(uiShaderVertex, 1, &str, NULL);
      CheckStatusVertex();

      glCompileShader(uiShaderVertex);
      CheckStatusVertex();

      if (IsCompiledVertex()) std::cout<<"cShader::_LoadVertexShader Vertex shader "<<opengl::string::ToUTF8(sShaderVertex)<<": Compiled"<<std::endl;
      else {
        std::cout<<"cShader::_LoadVertexShader Vertex shader "<<opengl::string::ToUTF8(sShaderVertex)<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      std::cout<<"cShader::_LoadVertexShader Shader not found "<<opengl::string::ToUTF8(sShaderVertex)<<std::endl;
      uiShaderVertex = 0;
    }
  }

  void cShader::_LoadFragmentShader(const opengl::string_t& _sShaderFragment)
  {
    sShaderFragment = _sShaderFragment;

    std::ifstream f(opengl::string::ToUTF8(sShaderFragment).c_str());
    if (f.is_open()) {
      std::ostringstream o;
      std::string sLine;
      while (!f.eof()) {
        std::getline(f, sLine);

        o<<sLine;
        o<<"\n";

        ParseShaderLine(sLine);
      };

      std::cout<<"cShader::_LoadFragmentShader Fragment shader=\""<<o.str()<<"\""<<std::endl;

      uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
      std::cout<<"cShader::_LoadFragmentShader Fragment shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusFragment();
      assert(uiShaderFragment != 0);

      const std::string sBuffer = o.str();
      const char* str = sBuffer.c_str();
      glShaderSource(uiShaderFragment, 1, &str, NULL);
      CheckStatusFragment();

      glCompileShader(uiShaderFragment);
      CheckStatusFragment();

      if (IsCompiledFragment()) std::cout<<"cShader::_LoadFragmentShader Fragment shader "<<opengl::string::ToUTF8(sShaderFragment)<<": Compiled"<<std::endl;
      else {
        std::cout<<"cShader::_LoadFragmentShader Fragment shader "<<opengl::string::ToUTF8(sShaderFragment)<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      std::cout<<"cShader::_LoadFragmentShader Shader not found "<<opengl::string::ToUTF8(sShaderFragment)<<std::endl;
      uiShaderFragment = 0;
    }
  }

  void cShader::_Compile()
  {
    if (IsCompiledVertex() || IsCompiledFragment()) {
      uiShaderProgram = glCreateProgram();
      std::cout<<"cShader::_Compile program glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusProgram();
      assert(uiShaderFragment != 0);

      if (IsCompiledVertex()) {
        glAttachShader(uiShaderProgram, uiShaderVertex);
        CheckStatusProgram();
      }
      if (IsCompiledFragment()) {
        glAttachShader(uiShaderProgram, uiShaderFragment);
        CheckStatusProgram();
      }

      glLinkProgram(uiShaderProgram);
      CheckStatusProgram();

      glUseProgram(uiShaderProgram);
      CheckStatusProgram();

      glUseProgram(0);
      CheckStatusProgram();
    }
  }


  bool cShader::LoadVertexShaderOnly(const opengl::string_t& _sShaderVertex)
  {
    std::cout<<"cShader::LoadVertexShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadVertexShader(_sShaderVertex);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadFragmentShaderOnly(const opengl::string_t& _sShaderFragment)
  {
    std::cout<<"cShader::LoadFragmentShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadFragmentShader(_sShaderFragment);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadVertexShaderAndFragmentShader(const opengl::string_t& _sShaderVertex, const opengl::string_t& _sShaderFragment)
  {
    std::cout<<"cShader::LoadVertexShaderAndFragmentShader glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadFragmentShader(_sShaderFragment);
    _LoadVertexShader(_sShaderVertex);
    _Compile();

    return IsCompiledProgram();
  }

  void cShader::Destroy()
  {
    if (uiShaderFragment != 0) {
      glDeleteShader(uiShaderFragment);
      uiShaderFragment = 0;
    }
    if (uiShaderVertex != 0) {
      glDeleteShader(uiShaderVertex);
      uiShaderVertex = 0;
    }

    glDeleteProgram(uiShaderProgram);
    uiShaderProgram = 0;
  }
}
