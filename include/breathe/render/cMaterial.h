#ifndef CMATERIAL_H
#define CMATERIAL_H

#include <spitfire/util/cSmartPtr.h>

#include <breathe/render/cTexture.h>

namespace breathe
{
  namespace render
  {
    class cShaderConstants
    {
    public:
      cShaderConstants() {}
      cShaderConstants(const cShaderConstants& rhs) { Assign(rhs); }

      cShaderConstants& operator=(const cShaderConstants& rhs) { Assign(rhs); return *this; }

      int GetValueInt(const string_t& sName) const;
      float GetValueFloat(const string_t& sName) const;
      math::cVec2 GetValueVec2(const string_t& sName) const;
      math::cVec3 GetValueVec3(const string_t& sName) const;
      math::cVec4 GetValueVec4(const string_t& sName) const;

      void SetValue(const string_t& sName, int value) { mInt[sName] = value; }
      void SetValue(const string_t& sName, float value) { mFloat[sName] = value; }
      void SetValue(const string_t& sName, const math::cVec2& value) { mVec2[sName] = value; }
      void SetValue(const string_t& sName, const math::cVec3& value) { mVec3[sName] = value; }
      void SetValue(const string_t& sName, const math::cVec4& value) { mVec4[sName] = value; }

    private:
      void Assign(const cShaderConstants& rhs);

      std::map<string_t, int> mInt;
      std::map<string_t, float> mFloat;
      std::map<string_t, math::cVec2> mVec2;
      std::map<string_t, math::cVec3> mVec3;
      std::map<string_t, math::cVec4> mVec4;
    };

    // void cRender::SetMaterial(material::cMaterialRef pMaterial);
    // void cRender::SetMaterial(material::cMaterialRef pMaterial, const cShaderConstants& shaderConstants);



    // A cShader is a combination of a either one or zero vertex program and one or zero fragment program
    class cShader
    {
    public:
      cShader();

      void Init();
      void Destroy();

      void CheckStatusVertex();
      void CheckStatusFragment();
      void CheckStatusProgram();

      bool IsCompiledVertex() const;
      bool IsCompiledFragment() const;
      bool IsCompiledProgram() const;

      string_t sShaderVertex;
      string_t sShaderFragment;

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
    };

    typedef cSmartPtr<cShader> cShaderRef;


    namespace material
    {
      const size_t nLayers = 3;

      class cLayer
      {
      public:
        cLayer();

        void CloneTo(cLayer& destination) const;

        cTextureRef pTexture;
        TEXTURE_MODE uiTextureMode;
        string_t sTexture;
      };

      class cMaterial;
      typedef cSmartPtr<cMaterial> cMaterialRef;

      class cMaterial
      {
      public:
        explicit cMaterial(const string_t& sName);
        ~cMaterial();

        bool Load(const string_t& sNewFilename);

        void CloneTo(cMaterialRef pDestination);

        // Rendering
        uint8_t chDustR;
        uint8_t chDustG;
        uint8_t chDustB;

        std::vector<cLayer*> vLayer;
        // 0=Diffuse
        // 1+=Either none, detail, cubemap or bumpmap

        // Lightmap Generation
        bool bShadow_cast;
        bool bShadow_receive;
        bool bLight_receive;
        bool bLight_transmit;

        // Audio
        unsigned int uiAudioScrape;
        unsigned int uiAudioBounce;

        // Physics
        bool bCollideTrimesh;

        float fFriction;
        float fBounce;

        float fCorrugation; // 0.0f=none, 1.0f=very bouncy (stairs etc.)

        string_t sName;

        cShaderRef pShader;
      };
    }
  }
}

#endif // CMATERIAL_H
