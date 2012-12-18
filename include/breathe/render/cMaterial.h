#ifndef CMATERIAL_H
#define CMATERIAL_H

// libopenglmm headers
#include <libopenglmm/cShader.h>

// Spitfire headers
#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/string.h>

// Breathe headers
#include <breathe/render/cTexture.h>

namespace breathe
{
  namespace render
  {
    typedef opengl::cShader cShader;
    typedef cShader* cShaderRef;

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


    namespace material
    {
      // using opengl::TEXTURE_TYPE;
      enum class TEXTURE_TYPE {
        NORMAL
      };

      const size_t nLayers = 3;

      class cLayer
      {
      public:
        cLayer();

        void CloneTo(cLayer& destination) const;

        cTextureRef pTexture;
        TEXTURE_TYPE uiTextureMode;
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

        bool IsLightEmitting() const { return bLight_emit; }
        bool IsLightReceiving() const { return bLight_receive; }
        bool IsShadowCasting() const { return bShadow_cast; }
        bool IsShadowReceiving() const { return bShadow_receive; }


        string_t sName;

        // Rendering
        uint8_t chDustR;
        uint8_t chDustG;
        uint8_t chDustB;

        std::vector<cLayer*> vLayer;
        // 0=Diffuse
        // 1+=Either none, detail, cubemap or bumpmap

        // Lightmap Generation
        bool bLight_emit;
        bool bLight_receive;
        bool bShadow_cast;
        bool bShadow_receive;

        // Audio
        unsigned int uiAudioScrape;
        unsigned int uiAudioBounce;

        // Physics
        bool bCollideTrimesh;

        float fFriction;
        float fBounce;

        float fCorrugation; // 0.0f=none, 1.0f=very bouncy (stairs etc.)

        cShaderRef pShader;
      };
    }
  }
}

#endif // CMATERIAL_H
