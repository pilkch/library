#ifndef CMATERIAL_H
#define CMATERIAL_H

namespace breathe
{
  namespace render
  {
    class cShaderConstants
    {
    public:
      void SetValue(const string_t& sName, int value) { mInt[sName] = value; }
      void SetValue(const string_t& sName, float value) { mFloat[sName] = value; }
      void SetValue(const string_t& sName, const math::cVec2& value) { mVec2[sName] = value; }
      void SetValue(const string_t& sName, const math::cVec3& value) { mVec3[sName] = value; }

    private:
      std::map<string_t, int> mInt;
      std::map<string_t, float> mFloat;
      std::map<string_t, math::cVec2> mVec2;
      std::map<string_t, math::cVec3> mVec3;
    };

    // void cRender::SetMaterial(material::cMaterialRef pMaterial);
    // void cRender::SetMaterial(material::cMaterialRef pMaterial, const cShaderConstants& shaderConstants);



    /*
    class cVertexProgram
    {
    public:
      void SetNamedValue(const string_t& variable, float value) { values_float[variable] = value; }
      void SetNamedValue(const string_t& variable, const math::cVec2& value) { values_cVec2[variable] = value; }
      void SetNamedValue(const string_t& variable, const math::cVec3& value) { values_cVec3[variable] = value; }
      void SetNamedValue(const string_t& variable, const math::cVec4& value) { values_cVec4[variable] = value; }
      void SetNamedValue(const string_t& variable, const math::cColour& value) { values_cColour[variable] = value; }

    private:
      void PassValuesToProgram();
      void PassFloatValueToProgram(const string_t& name, float value);

      std::map<string_t, float> values_float;
      std::map<string_t, math::cVec2> values_cVec2;
      std::map<string_t, math::cVec3> values_cVec3;
      std::map<string_t, math::cVec4> values_cVec4;
      std::map<string_t, math::cColour> values_cColour;
    };

    cVertexProgram::PassValuesToProgram()
    {
      std::map<string_t, float>::iterator iter(values_float.begin());
      const std::map<string_t, float>::iterator iterEnd(variables_float.end());

      while (iter != iterEnd) {
        PassFloatToProgram(iter->first, iter->second);
        iter++;
      }
    }
    */

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

      bool bTexUnit0;
      bool bTexUnit1;
      bool bTexUnit2;
      bool bTexUnit3;

      unsigned int uiShaderVertex;
      unsigned int uiShaderFragment;

      // Combined resource id
      unsigned int uiShaderProgram;

      bool bCameraPos;
    };

    typedef cSmartPtr<cShader> cShaderRef;


    namespace material
    {
      const size_t nLayers = 3;

      class cLayer
      {
      public:
        cLayer();

        cTextureRef pTexture;
        unsigned int uiTextureMode;
        string_t sTexture;
      };

      class cMaterial
      {
      public:
        explicit cMaterial(const string_t& sName);
        ~cMaterial();

        bool Load(const string_t& sNewFilename);

        // Rendering
        uint8_t chDustR;
        uint8_t chDustG;
        uint8_t chDustB;

        std::vector<cLayer*>vLayer;
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

      typedef cSmartPtr<cMaterial> cMaterialRef;
    }
  }
}

#endif // CMATERIAL_H
