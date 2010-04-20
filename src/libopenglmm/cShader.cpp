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
#include <vector>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>

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

    bTexUnit0(false),
    bTexUnit1(false),
    bTexUnit2(false),
    bTexUnit3(false),
    bCameraPos(false),
    bAmbientColour(false),
    bLightPosition(false)
  {
  }

  void LoadVertexShaderOnly(const std::string& sShaderVertex);
  void LoadFragmentShaderOnly(const std::string& sShaderFragment);
  void LoadVertexShaderAndFragmentShader(const std::string& sShaderVertex, const std::string& sShaderFragment);

  void cShader::CheckStatusVertex()
  {
    std::cout<<"cShader::CheckStatusVertex Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
    std::cout<<"cShader::CheckStatusVertex glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      glGetShaderInfoLog(uiShaderVertex, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusVertex "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      )
      {
        sInfo = StringReplace(sInfo, "\n", "<br>");
        std::cout<<"cShader::CheckStatusVertex Vertex Shader "<<sShaderVertex<<": "<<sInfo<<std::endl;
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
      char *infoLog = new char[infologLength];
      glGetShaderInfoLog(uiShaderFragment, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusFragment "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      )
      {
        sInfo = StringReplace(sInfo, "\n", "<br>");
        std::cout<<"cShader::CheckStatusFragment  Fragment Shader "<<sShaderFragment<<": "<<sInfo<<std::endl;
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
      char *infoLog = new char[infologLength];
      glGetProgramInfoLog(uiShaderProgram, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      std::cout<<"cShader::CheckStatusProgram "<<sInfo<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("Warning") != std::string::npos
      ) {
        std::cout<<"cShader::CheckStatusProgram Program "<<sShaderVertex<<" "<<sShaderFragment<<": "<<infoLog<<std::endl;
      } else {
        std::cout<<"cShader::CheckStatusProgram Program "<<sShaderVertex<<" "<<sShaderFragment<<": "<<infoLog<<std::endl;
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

  void cShader::_LoadVertexShader(const std::string& _sShaderVertex)
  {
    sShaderVertex = _sShaderVertex;

    std::string buffer = "";
    std::string line = "";
    std::ifstream f(sShaderVertex.c_str());
    if (f.is_open()) {
      while (!f.eof()) {
        std::getline(f, line);

        buffer += line;
        buffer += "\n";

        line = "";
      };

      std::cout<<"cShader::_LoadVertexShader Vertex "<<cSystem::GetErrorString()<<" "<<cSystem::GetErrorString()<<" shader=\""<<buffer<<"\""<<std::endl;

      uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);
      std::cout<<"cShader::_LoadVertexShader Vertex shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusVertex();
      assert(uiShaderVertex != 0);

      const char* str = buffer.c_str();
      glShaderSource(uiShaderVertex, 1, &str, NULL);
      CheckStatusVertex();

      glCompileShader(uiShaderVertex);
      CheckStatusVertex();

      if (IsCompiledVertex()) std::cout<<"cShader::_LoadVertexShader Vertex shader "<<sShaderVertex<<": Compiled"<<std::endl;
      else {
        std::cout<<"cShader::_LoadVertexShader Vertex shader "<<sShaderVertex<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      std::cout<<"cShader::_LoadVertexShader Shader not found "<<sShaderVertex<<std::endl;
      uiShaderVertex = 0;
    }
  }

  void cShader::_LoadFragmentShader(const std::string& _sShaderFragment)
  {
    sShaderFragment = _sShaderFragment;

    std::string buffer;
    std::string line;
    std::ifstream f(sShaderFragment.c_str());
    if (f.is_open()) {
      while (!f.eof()) {
        std::getline(f, line);

        buffer += line;
        buffer += "\n";
      };

      std::cout<<"cShader::_LoadFragmentShader Fragment shader=\""<<buffer<<"\""<<std::endl;

      uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
      std::cout<<"cShader::_LoadFragmentShader Fragment shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusFragment();
      assert(uiShaderFragment != 0);

      const char* str = buffer.c_str();
      glShaderSource(uiShaderFragment, 1, &str, NULL);
      CheckStatusFragment();

      glCompileShader(uiShaderFragment);
      CheckStatusFragment();

      if (IsCompiledFragment()) std::cout<<"cShader::_LoadFragmentShader Fragment shader "<<sShaderFragment<<": Compiled"<<std::endl;
      else {
        std::cout<<"cShader::_LoadFragmentShader Fragment shader "<<sShaderFragment<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      std::cout<<"cShader::_LoadFragmentShader Shader not found "<<sShaderFragment<<std::endl;
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


  bool cShader::LoadVertexShaderOnly(const std::string& _sShaderVertex)
  {
    std::cout<<"cShader::LoadVertexShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadVertexShader(_sShaderVertex);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadFragmentShaderOnly(const std::string& _sShaderFragment)
  {
    std::cout<<"cShader::LoadFragmentShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadFragmentShader(_sShaderFragment);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadVertexShaderAndFragmentShader(const std::string& _sShaderVertex, const std::string& _sShaderFragment)
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