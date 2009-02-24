#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

// Writing to and from text files
#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>
#include <map>
#include <string>

// Boost headers
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>


// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/game/cLevel.h>

namespace breathe
{
  namespace render
  {
    namespace material
    {
      cLayer::cLayer() :
        uiTextureMode(TEXTURE_NONE)
      {
      }


      cShader::cShader() :
        bTexUnit0(false),
        bTexUnit1(false),
        bTexUnit2(false),
        bTexUnit3(false),

        uiShaderVertex(0),
        uiShaderFragment(0),
        uiShaderProgram(0),

        bCameraPos(false)
      {
      }


      void cShader::CheckStatusVertex()
      {
        LOG<<"cShader::CheckStatusVertex Last error="<<pRender->GetErrorString()<<std::endl;

        int infologLength = 0;
        glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
        LOG<<"cShader::CheckStatusVertex glGetShaderiv glGetError="<<pRender->GetErrorString()<<std::endl;
        infologLength = 1024;
        if (infologLength > 0) {
          char* infoLog = new char[infologLength];
          glGetShaderInfoLog(uiShaderVertex, infologLength, NULL, infoLog);
          std::string sInfo(infoLog);
          LOG<<"cShader::CheckStatusVertex "<<sInfo<<std::endl;
          if (
            sInfo.find("not been successfully compiled") != std::string::npos ||
            sInfo.find("ERROR") != std::string::npos
          )
          {
            sInfo = string::Replace(sInfo, "\n", "<br>");
            LOG.Error("Material", std::string("Vertex Shader") + breathe::string::ToUTF8(sShaderVertex) + std::string(": ") + sInfo);
          }
          SAFE_DELETE_ARRAY(infoLog);
        }

        LOG<<"cShader::CheckStatusVertex returning"<<std::endl;
      }

      void cShader::CheckStatusFragment()
      {
        LOG<<"cShader::CheckStatusFragment Last error="<<pRender->GetErrorString()<<std::endl;

        int infologLength = 0;
        glGetShaderiv(uiShaderFragment, GL_INFO_LOG_LENGTH, &infologLength);
        LOG<<"cShader::CheckStatusFragment glGetShaderiv glGetError="<<pRender->GetErrorString()<<std::endl;
        infologLength = 1024;
        if (infologLength > 0) {
          char *infoLog = new char[infologLength];
          glGetShaderInfoLog(uiShaderFragment, infologLength, NULL, infoLog);
          std::string sInfo(infoLog);
          LOG<<"cShader::CheckStatusFragment "<<sInfo<<std::endl;
          if (
            sInfo.find("not been successfully compiled") != std::string::npos ||
            sInfo.find("ERROR") != std::string::npos
          )
          {
            sInfo = string::Replace(sInfo, "\n", "<br>");
            LOG.Error("Material", std::string("Fragment Shader ") + breathe::string::ToUTF8(sShaderFragment) + ": " + sInfo);
          }
          SAFE_DELETE_ARRAY(infoLog);
        }

        LOG<<"cShader::CheckStatusFragment returning"<<std::endl;
      }

      void cShader::CheckStatusProgram()
      {
        LOG<<"cShader::CheckStatusProgram Last error="<<pRender->GetErrorString()<<std::endl;

        int infologLength = 0;
        glGetProgramiv(uiShaderProgram, GL_INFO_LOG_LENGTH, &infologLength);
        LOG<<"cShader::CheckStatusProgram glGetShaderiv glGetError="<<pRender->GetErrorString()<<std::endl;
        infologLength = 1024;
        if (infologLength > 0) {
          char *infoLog = new char[infologLength];
          glGetProgramInfoLog(uiShaderProgram, infologLength, NULL, infoLog);
          std::string sInfo(infoLog);
          LOG<<"cShader::CheckStatusProgram "<<sInfo<<std::endl;
          if (
            sInfo.find("not been successfully compiled") != std::string::npos ||
            sInfo.find("Warning") != std::string::npos
          ) {
            LOG.Error("Material", std::string("Program ") + breathe::string::ToUTF8(sShaderVertex) + " " + breathe::string::ToUTF8(sShaderFragment) + ": " + infoLog);
          } else {
            LOG.Success("Material", std::string("Program ") + breathe::string::ToUTF8(sShaderVertex) + " " + breathe::string::ToUTF8(sShaderFragment) + ": " + infoLog);
          }
          SAFE_DELETE_ARRAY(infoLog);
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

      void cShader::Init()
      {
        LOG<<"cShader::Init glGetError="<<pRender->GetErrorString()<<std::endl;

        if (!sShaderVertex.empty()) {
          std::string buffer = "";
          std::string line = "";
          std::ifstream f(breathe::string::ToUTF8(sShaderVertex).c_str());
          if (f.is_open()) {
            while (!f.eof()) {
              std::getline(f, line);

              buffer += line;
              buffer += "\n";

              line = "";
            };

            LOG<<"cShader::Init Vertex "<<pRender->GetErrorString()<<" "<<pRender->GetErrorString()<<" shader=\""<<buffer<<"\""<<std::endl;

            uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);
            LOG<<"cShader::Init Vertex shader glGetError="<<pRender->GetErrorString()<<std::endl;
            ASSERT(uiShaderVertex != 0);

            const char* str = buffer.c_str();
            glShaderSource(uiShaderVertex, 1, &str, NULL);
            CheckStatusVertex();

            glCompileShader(uiShaderVertex);
            CheckStatusVertex();

            if (IsCompiledVertex()) LOG.Success("cShader::Init", std::string("Vertex shader ") + breathe::string::ToUTF8(sShaderVertex) + ": Compiled");
            else LOG.Error("cShader::Init", std::string("Vertex shader ") + breathe::string::ToUTF8(sShaderVertex) + ": Not compiled");
          } else {
            LOG.Error("Material", std::string("Shader not found ") + breathe::string::ToUTF8(sShaderVertex));
            uiShaderVertex = 0;
          }
        }


        if (!sShaderFragment.empty()) {
          std::string buffer;
          std::string line;
          std::ifstream f(breathe::string::ToUTF8(sShaderFragment).c_str());
          if (f.is_open()) {
            while (!f.eof()) {
              std::getline(f, line);

              buffer += line;
              buffer += "\n";
            };

            LOG<<"cShader::Init Fragment shader=\""<<buffer<<"\""<<std::endl;

            uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
            LOG<<"cShader::Init Fragment shader glGetError="<<pRender->GetErrorString()<<std::endl;
            ASSERT(uiShaderFragment != 0);

            const char* str = buffer.c_str();
            glShaderSource(uiShaderFragment, 1, &str, NULL);
            CheckStatusFragment();

            glCompileShader(uiShaderFragment);
            CheckStatusFragment();

            if (IsCompiledFragment()) LOG.Success("cShader::Init", std::string("Fragment shader ") + breathe::string::ToUTF8(sShaderFragment) + ": Compiled");
            else LOG.Error("cShader::Init", std::string("Fragment shader ") + breathe::string::ToUTF8(sShaderFragment) + ": Not compiled");
          } else {
            LOG.Error("Material", std::string("Shader not found ") + breathe::string::ToUTF8(sShaderFragment));
            uiShaderFragment = 0;
          }
        }

        if (IsCompiledVertex() || IsCompiledFragment()) {
          uiShaderProgram = glCreateProgram();
          LOG<<"cShader::Init program glGetError="<<pRender->GetErrorString()<<std::endl;
          CheckStatusProgram();
          ASSERT(uiShaderFragment != 0);

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

      void cShader::Destroy()
      {
        if (uiShaderFragment) glDeleteShader(uiShaderFragment);
        if (uiShaderVertex) glDeleteShader(uiShaderVertex);

        glDeleteProgram(uiShaderProgram);

        uiShaderFragment = 0;
        uiShaderVertex = 0;
        uiShaderProgram = 0;
      }


      cMaterial::cMaterial(const string_t& name) :
        chDustR(0),
        chDustG(0),
        chDustB(0),

        bShadow_cast(true),
        bShadow_receive(true),
        bLight_receive(true),
        bLight_transmit(true),

        uiAudioScrape(0),
        uiAudioBounce(0),

        bCollideTrimesh(false),

        fFriction(0.0f),
        fBounce(0.0f),

        fCorrugation(0.0f),

        sName(name),

        pShader(NULL)
      {
        unsigned int i = 0;
        for (i=0;i<nLayers;i++) vLayer.push_back(new cLayer());
      }

      cMaterial::~cMaterial()
      {
        unsigned int i=0;
        for (i=0;i<nLayers;i++) SAFE_DELETE(vLayer[i]);
      }

      bool cMaterial::Load(const string_t& inFilename)
      {
        LOG.Success("Material", std::string("Looking for ") + breathe::string::ToUTF8(inFilename));

        string_t sFilename = inFilename;

        string_t sPath = breathe::filesystem::GetPath(sFilename);

        xml::cNode root(sFilename);
        xml::cNode::iterator iter(root);

        if (!iter.IsValid()) return breathe::BAD;

        //<material collide="true" sShaderVertex="normalmap.vert" sShaderFragment="normalmap.frag">
        //  <layer sTexture="concrete.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
        //  <layer sTexture="concrete_normalmap.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
        //</material>

        iter.FindChild("material");
        if (!iter.IsValid()) {
          LOG.Error("Material", std::string("Not Found ") + breathe::string::ToUTF8(sFilename));
          for (unsigned int i=0;i<nLayers;i++)
            vLayer[i]->pTexture = pRender->pMaterialNotFoundMaterial->vLayer[0]->pTexture;

          return breathe::BAD;
        }

        iter.GetAttribute("collide", bCollideTrimesh);

        std::string sValue;
        if (iter.GetAttribute("sShaderVertex", sValue)) {
          LOG<<"cMaterial::Load Vertex shader found \""<<sValue<<"\""<<std::endl;

          if (pShader == nullptr) pShader = new cShader;

          // Try in the same directory as the material
          if (!breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sValue), pShader->sShaderVertex)) {
            // Failed, let's try the default shader directory
            if (!breathe::filesystem::FindResourceFile(TEXT("shaders/"), breathe::string::ToString_t(sValue), pShader->sShaderVertex)) {
              LOG.Error("Material", std::string("Shader Not Found ") + breathe::string::ToUTF8(sPath) + breathe::string::ToUTF8(sValue));
            }
          }
        }
        if (iter.GetAttribute("sShaderFragment", sValue)) {
          LOG<<"cMaterial::Load Fragment shader found \""<<sValue<<"\""<<std::endl;

          if (pShader == nullptr) pShader = new cShader;

          // Try in the same directory as the material
          if (!breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sValue), pShader->sShaderFragment)) {
            // Failed, let's try the default shader directory
            if (!breathe::filesystem::FindResourceFile(TEXT("shaders/"), breathe::string::ToString_t(sValue), pShader->sShaderFragment)) {
              LOG.Error("Material", std::string("Shader Not Found ") + breathe::string::ToUTF8(sPath) + breathe::string::ToUTF8(sValue));
            }
          }
        }

        if (pShader != nullptr) {
          iter.GetAttribute("cameraPos", pShader->bCameraPos);

          iter.GetAttribute("texUnit0", pShader->bTexUnit0);
          iter.GetAttribute("texUnit1", pShader->bTexUnit1);
          iter.GetAttribute("texUnit2", pShader->bTexUnit2);
          iter.GetAttribute("texUnit3", pShader->bTexUnit3);

          pShader->Init();
        }


        iter.FirstChild();

        cLayer* pLayer = nullptr;
        size_t i = 0;
        while ((iter.IsValid()) && (i < nLayers)) {
          pLayer = vLayer[i];
          if ("layer" == iter.GetName()) {
            std::string sTexture;
            if (iter.GetAttribute("sTexture", sTexture)) {
              // Try in the same directory as the material
              if (!breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sTexture), pLayer->sTexture)) {
                // Failed, let's try the default texture directory
                if (!breathe::filesystem::FindResourceFile(TEXT("textures/"), breathe::string::ToString_t(sTexture), pLayer->sTexture)) {
                  pLayer->sTexture = breathe::string::ToString_t(sTexture);
                }
              }
            }

            LOG<<"material Loading texture \""<<pLayer->sTexture<<"\""<<std::endl;

            std::string sValue;
            if (iter.GetAttribute("uiTextureMode", sValue)) {
              if (sValue == "TEXTURE_NORMAL")            pLayer->uiTextureMode=TEXTURE_NORMAL;
              else if (sValue == "TEXTURE_MASK")          pLayer->uiTextureMode=TEXTURE_MASK;
              else if (sValue == "TEXTURE_BLEND")        pLayer->uiTextureMode=TEXTURE_BLEND;
              else if (sValue == "TEXTURE_DETAIL")        pLayer->uiTextureMode=TEXTURE_DETAIL;
              else if (sValue == "TEXTURE_CUBEMAP")      pLayer->uiTextureMode=TEXTURE_CUBEMAP;
              else if (sValue == "TEXTURE_POST_RENDER")  pLayer->uiTextureMode=TEXTURE_POST_RENDER;
            }

            unsigned int uiTextureAtlas = ATLAS_NONE;
            if (iter.GetAttribute("uiTextureAtlas", sValue)) {
              if (sValue == "ATLAS_LANDSCAPE")      uiTextureAtlas = ATLAS_LANDSCAPE;
              else if (sValue == "ATLAS_BUILDING")  uiTextureAtlas = ATLAS_BUILDING;
              else if (sValue == "ATLAS_FOLIAGE")  uiTextureAtlas = ATLAS_FOLIAGE;
              else if (sValue == "ATLAS_VEHICLES")  uiTextureAtlas = ATLAS_VEHICLES;
              else if (sValue == "ATLAS_PROPS")    uiTextureAtlas = ATLAS_PROPS;
              else if (sValue == "ATLAS_WEAPONS")  uiTextureAtlas = ATLAS_WEAPONS;
              else if (sValue == "ATLAS_EFFECTS")  uiTextureAtlas = ATLAS_EFFECTS;
            }

            if (TEXTURE_CUBEMAP == pLayer->uiTextureMode) {
              uiTextureAtlas = ATLAS_NONE;
              LOG.Error("CUBEMAP", "CUBEMAP");
            }

            if ((TEXTURE_CUBEMAP != pLayer->uiTextureMode) && (TEXTURE_POST_RENDER != pLayer->uiTextureMode)) {
              if (ATLAS_NONE != uiTextureAtlas) pLayer->pTexture = pRender->AddTextureToAtlas(pLayer->sTexture, uiTextureAtlas);

              if (nullptr == pLayer->pTexture) {
                uiTextureAtlas = ATLAS_NONE;
                pLayer->pTexture = pRender->AddTexture(pLayer->sTexture);
              }
            }
          }

          i++;
          iter++;
        }

        LOG.Success("Material", breathe::string::ToUTF8(TEXT("Loaded ") + sFilename));
        return breathe::GOOD;
      }

    }
  }
}
