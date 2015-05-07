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
#include <SDL2/SDL_image.h>

// Spitfire headers
#include <spitfire/storage/filesystem.h>
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>

// libopenglmm headers
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/opengl.h>

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
    LOG<<"cShader::CheckStatusVertex Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
    LOG<<"cShader::CheckStatusVertex glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetShaderInfoLog(uiShaderVertex, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      LOG<<"cShader::CheckStatusVertex "<<spitfire::string::ToString_t(sInfo)<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      ) {
        sInfo = spitfire::string::Replace(sInfo, "\n", "<br>");
        LOG<<TEXT("cShader::CheckStatusVertex Vertex Shader ")<<sShaderVertex<<TEXT(": ")<<spitfire::string::ToString_t(sInfo)<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    LOG<<"cShader::CheckStatusVertex returning"<<std::endl;
  }

  void cShader::CheckStatusFragment()
  {
    LOG<<"cShader::CheckStatusFragment Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetShaderiv(uiShaderFragment, GL_INFO_LOG_LENGTH, &infologLength);
    LOG<<"cShader::CheckStatusFragment glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetShaderInfoLog(uiShaderFragment, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      LOG<<"cShader::CheckStatusFragment "<<spitfire::string::ToString_t(sInfo)<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("ERROR") != std::string::npos
      ) {
        sInfo = spitfire::string::Replace(sInfo, "\n", "<br>");
        LOG<<"cShader::CheckStatusFragment  Fragment Shader "<<sShaderFragment<<": "<<spitfire::string::ToString_t(sInfo)<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    LOG<<"cShader::CheckStatusFragment returning"<<std::endl;
  }

  void cShader::CheckStatusProgram()
  {
    LOG<<"cShader::CheckStatusProgram Last error="<<cSystem::GetErrorString()<<std::endl;

    int infologLength = 0;
    glGetProgramiv(uiShaderProgram, GL_INFO_LOG_LENGTH, &infologLength);
    LOG<<"cShader::CheckStatusProgram glGetShaderiv glGetError="<<cSystem::GetErrorString()<<std::endl;
    //infologLength = 1024;
    if (infologLength > 0) {
      char* infoLog = new char[infologLength];
      infoLog[0] = 0;
      glGetProgramInfoLog(uiShaderProgram, infologLength, NULL, infoLog);
      std::string sInfo(infoLog);
      LOG<<"cShader::CheckStatusProgram "<<spitfire::string::ToString_t(sInfo)<<std::endl;
      if (
        sInfo.find("not been successfully compiled") != std::string::npos ||
        sInfo.find("Warning") != std::string::npos
      ) {
        LOG<<"cShader::CheckStatusProgram Program "<<spitfire::string::ToString_t(sShaderVertex)<<" "<<spitfire::string::ToString_t(sShaderFragment)<<": "<<spitfire::string::ToString_t(infoLog)<<std::endl;
      } else {
        LOG<<"cShader::CheckStatusProgram Program "<<spitfire::string::ToString_t(sShaderVertex)<<" "<<spitfire::string::ToString_t(sShaderFragment)<<": "<<spitfire::string::ToString_t(infoLog)<<std::endl;
      }
      delete [] infoLog;
      infoLog = nullptr;
    }

    LOG<<"cShader::CheckStatusProgram returning"<<std::endl;
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

  void cShader::ParseLineShader(const std::string& sLine)
  {
    // Warn about deprecated OpenGL 2 built in variables
    if (sLine.find("gl_ModelViewProjectionMatrix") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_ModelViewProjectionMatrix\" should be replaced with \"uniform mat4 matModelViewProjection;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_NormalMatrix") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_NormalMatrix\" should be replaced with \"uniform mat3 matNormal;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.ambient") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_FrontMaterial.ambient\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.diffuse") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_FrontMaterial.diffuse\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.specular") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_FrontMaterial.specular\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_FrontMaterial.shininess") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_FrontMaterial.shininess\" should be replaced with a uniform on the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_LightModel.ambient") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_LightModel.ambient\" should be replaced with \"uniform vec4 ambientColour;\" in the shader"<<std::endl;
      assert(false);
    } else if (sLine.find("gl_LightSource") != std::string::npos) {
      LOGERROR<<"cShader::ParseLineShader \"gl_LightSource\" should be replaced with uniforms on the shader"<<std::endl;
      assert(false);
    }

    // Check which uniforms this shader uses
    if (spitfire::string::StartsWith(sLine, "uniform mat4 matProjection;")) bProjectionMatrix = true;
    else if (spitfire::string::StartsWith(sLine, "uniform mat4 matModelView;")) bModelViewMatrix = true;
    else if (spitfire::string::StartsWith(sLine, "uniform mat4 matModelViewProjection;")) bModelViewProjectionMatrix = true;
    else if (spitfire::string::StartsWith(sLine, "uniform mat3 matNormal;")) bNormalMatrix = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2D texUnit0;")) bTexUnit0 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2D texUnit1;")) bTexUnit1 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2D texUnit2;")) bTexUnit2 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2D texUnit3;")) bTexUnit3 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2DRect texUnit0;")) bTexUnit0 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2DRect texUnit1;")) bTexUnit1 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2DRect texUnit2;")) bTexUnit2 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform sampler2DRect texUnit3;")) bTexUnit3 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform samplerCube texUnit0;")) bTexUnit0 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform samplerCube texUnit1;")) bTexUnit1 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform samplerCube texUnit2;")) bTexUnit2 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform samplerCube texUnit3;")) bTexUnit3 = true;
    else if (spitfire::string::StartsWith(sLine, "uniform vec4 ambientColour;")) bAmbientColour = true;
    else if (spitfire::string::StartsWith(sLine, "uniform vec3 sunPosition;")) bSunPosition = true;
    else if (spitfire::string::StartsWith(sLine, "uniform vec4 sunAmbientColour;")) bSunAmbientColour = true;
    else if (spitfire::string::StartsWith(sLine, "uniform float fSunIntensity;")) bSunIntensity = true;
  }

  void cShader::ParseLineVertexShader(const std::string& sLine)
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    // "in vec3 vertexColour;"
    if (spitfire::string::StartsWith(sLine, "in ")) {
      std::string::size_type space = sLine.find(" ", 3);
      if (space != std::string::npos) {
        // Skip the space
        space++;

        std::string::size_type semiColon = sLine.find(";");
        if (semiColon != std::string::npos) {
          const std::string sName(sLine.substr(space, semiColon - space));
          vAttributes.push_back(sName);
          LOG<<"cShader::ParseLineVertexShader \""<<spitfire::string::ToString_t(sName)<<"\""<<std::endl;
        }
      }
    }
    #endif
  }

  void cShader::ParseLineFragmentShader(const std::string& sLine)
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    // "out vec3 fragmentColour;"
    if (spitfire::string::StartsWith(sLine, "out ")) {
      std::string::size_type space = sLine.find(" ", 4);
      if (space != std::string::npos) {
        // Skip the space
        space++;

        std::string::size_type semiColon = sLine.find(";");
        if (semiColon != std::string::npos) {
          const std::string sName(sLine.substr(space, semiColon - space));
          vFragmentDataLocations.push_back(sName);
          LOG<<"cShader::ParseLineFragmentShader \""<<spitfire::string::ToString_t(sName)<<"\""<<std::endl;
        }
      }
    }
    #endif
  }

  size_t cShader::ParseVersion(const std::string& sLine) const
  {
    spitfire::string::cStringParserUTF8 sp(sLine);
    sp.SkipToStringAndSkip("#version");
    sp.SkipWhiteSpace();
    const std::string sVersionNumber = sp.GetToEnd();
    return spitfire::string::ToInt(spitfire::string::ToString_t(sVersionNumber));
  }

  // NOTE: This does not do the normal vertex/fragment shader parsing for variables,
  // 1) It is assumed that includes only contain global functions, no global inputs, outputs or other constructs
  // 2) It only processes one level of includes and is not recursive
  // TODO: Allow recursive includes
  std::string cShader::ParseInclude(const opengl::string_t& _sCurrentShaderPath, size_t uParentShaderVersion, const std::string& sIncludeLine) const
  {
    // Parse the include line
    spitfire::string::cStringParserUTF8 sp(sIncludeLine);
    sp.SkipToStringAndSkip("<");
    std::string sRelativeFilePath;
    sp.GetToString(">", sRelativeFilePath);

    // Read the whole included file
    #ifdef __WIN__
    const opengl::string_t sCurrentShaderPath = spitfire::string::Replace(_sCurrentShaderPath, TEXT("/"), TEXT("\\"));
    #else
    const opengl::string_t& sCurrentShaderPath = _sCurrentShaderPath;
    #endif
    const opengl::string_t sFilePath = spitfire::filesystem::MakeFilePath(spitfire::filesystem::GetFolder(sCurrentShaderPath), spitfire::string::ToString_t(sRelativeFilePath));
    std::ifstream f(spitfire::string::ToUTF8(sFilePath).c_str());
    if (!f.is_open()) {
      LOGERROR<<"cShader::ParseInclude Include not found \""<<sFilePath<<"\" for shader \""<<_sCurrentShaderPath<<"\""<<std::endl;
      return "";
    }

    std::ostringstream o;
    std::string sLine;
    while (!f.eof()) {
      std::getline(f, sLine);

      // Skip version lines (Assume they are the same as the calling shader)
      // TODO: Check the version of the include matches the version of the calling shader
      if (spitfire::string::StartsWith(sLine, "#version")) {
        const size_t uShaderVersion = ParseVersion(sLine);
        if (uShaderVersion != uParentShaderVersion) LOGERROR<<"cShader::ParseInclude Parent shader \""<<_sCurrentShaderPath<<"\" version "<<uParentShaderVersion<<" doesn't match included shader \""<<sFilePath<<"\" version "<<uShaderVersion<<""<<std::endl;

        continue;
      }

      o<<sLine;
      o<<"\n";
    };

    return o.str();
  }

  void cShader::_LoadVertexShader(const opengl::string_t& _sShaderVertex)
  {
    sShaderVertex = _sShaderVertex;

    std::ifstream f(opengl::string::ToUTF8(sShaderVertex).c_str());
    if (f.is_open()) {
      size_t uShaderVersion = 0;

      std::ostringstream o;
      std::string sLine;
      while (!f.eof()) {
        std::getline(f, sLine);

        // Check if this is a version line
        if (spitfire::string::StartsWith(sLine, "#version")) uShaderVersion = ParseVersion(sLine);

        if (spitfire::string::StartsWith(sLine, "#include <")) {
          const std::string sLines = ParseInclude(sShaderVertex, uShaderVersion, sLine);

          // Add the lines
          o<<sLines;
          o<<"\n";
        } else {
          ParseLineVertexShader(sLine);
          ParseLineShader(sLine);

          // Add the line
          o<<sLine;
          o<<"\n";
        }
      };

      LOG<<"cShader::_LoadVertexShader Vertex "<<cSystem::GetErrorString()<<" shader=\""<<spitfire::string::ToString_t(o.str())<<"\""<<std::endl;

      uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);
      LOG<<"cShader::_LoadVertexShader Vertex shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusVertex();
      assert(uiShaderVertex != 0);

      const std::string sBuffer = o.str();
      const char* str = sBuffer.c_str();
      glShaderSource(uiShaderVertex, 1, &str, NULL);
      CheckStatusVertex();

      glCompileShader(uiShaderVertex);
      CheckStatusVertex();

      if (IsCompiledVertex()) LOG<<"cShader::_LoadVertexShader Vertex shader "<<sShaderVertex<<": Compiled"<<std::endl;
      else {
        LOGERROR<<"cShader::_LoadVertexShader Vertex shader "<<sShaderVertex<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      LOGERROR<<"cShader::_LoadVertexShader Shader not found "<<sShaderVertex<<std::endl;
      uiShaderVertex = 0;
    }
  }

  void cShader::_LoadFragmentShader(const opengl::string_t& _sShaderFragment)
  {
    sShaderFragment = _sShaderFragment;

    std::ifstream f(opengl::string::ToUTF8(sShaderFragment).c_str());
    if (f.is_open()) {
      size_t uShaderVersion = 0;

      std::ostringstream o;
      std::string sLine;
      while (!f.eof()) {
        std::getline(f, sLine);

        // Check if this is a version line
        if (spitfire::string::StartsWith(sLine, "#version")) uShaderVersion = ParseVersion(sLine);

        if (spitfire::string::StartsWith(sLine, "#include <")) {
          const std::string sLines = ParseInclude(sShaderVertex, uShaderVersion, sLine);

          // Add the lines
          o<<sLines;
          o<<"\n";
        } else {
          ParseLineFragmentShader(sLine);
          ParseLineShader(sLine);

          // Add the line
          o<<sLine;
          o<<"\n";
        }
      };

      LOG<<"cShader::_LoadFragmentShader Fragment shader=\""<<spitfire::string::ToString_t(o.str())<<"\""<<std::endl;

      uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
      LOG<<"cShader::_LoadFragmentShader Fragment shader glGetError="<<cSystem::GetErrorString()<<std::endl;
      CheckStatusFragment();
      assert(uiShaderFragment != 0);

      const std::string sBuffer = o.str();
      const char* str = sBuffer.c_str();
      glShaderSource(uiShaderFragment, 1, &str, NULL);
      CheckStatusFragment();

      glCompileShader(uiShaderFragment);
      CheckStatusFragment();

      if (IsCompiledFragment()) LOG<<"cShader::_LoadFragmentShader Fragment shader "<<sShaderFragment<<": Compiled"<<std::endl;
      else {
        LOGERROR<<"cShader::_LoadFragmentShader Fragment shader "<<sShaderFragment<<": Not compiled"<<std::endl;
        assert(false);
      }
    } else {
      LOGERROR<<"cShader::_LoadFragmentShader Shader not found "<<sShaderFragment<<std::endl;
      uiShaderFragment = 0;
    }
  }

  void cShader::_Compile()
  {
    if (IsCompiledVertex() || IsCompiledFragment()) {
      uiShaderProgram = glCreateProgram();
      LOG<<"cShader::_Compile program glGetError="<<cSystem::GetErrorString()<<std::endl;
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
    LOG<<"cShader::LoadVertexShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadVertexShader(_sShaderVertex);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadFragmentShaderOnly(const opengl::string_t& _sShaderFragment)
  {
    LOG<<"cShader::LoadFragmentShaderOnly glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadFragmentShader(_sShaderFragment);
    _Compile();

    return IsCompiledProgram();
  }

  bool cShader::LoadVertexShaderAndFragmentShader(const opengl::string_t& _sShaderVertex, const opengl::string_t& _sShaderFragment)
  {
    LOG<<"cShader::LoadVertexShaderAndFragmentShader glGetError="<<cSystem::GetErrorString()<<std::endl;

    _LoadVertexShader(_sShaderVertex);
    _LoadFragmentShader(_sShaderFragment);
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

  void cShader::Bind()
  {
    glUseProgram(uiShaderProgram);

    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    // Set the attributes of the vertex shader
    const size_t nAttributes = vAttributes.size();
    for (size_t i = 0; i < nAttributes; i++) glBindAttribLocation(uiShaderProgram, (unsigned int)i, vAttributes[i].c_str());

    // Set the output parameters of the fragment shader
    const size_t nFragmentDataLocations = vFragmentDataLocations.size();
    for (size_t i = 0; i < nFragmentDataLocations; i++) glBindFragDataLocation(uiShaderProgram, (unsigned int)i, vFragmentDataLocations[i].c_str());
    #endif
  }

  void cShader::UnBind()
  {
    glUseProgram(0);
  }
}
