#ifndef CMD3_H
#define CMD3_H

#include <breathe/render/model/cMd3Loader.h>

namespace breathe
{
  namespace character
  {
    //class cModel
    //{
    //};

    //typedef cSmartPtr<cModel> cModelRef;

    class cAnimationPart
    {
    public:
      friend class cAnimation;

      cAnimationPart();

      // This is where we know exactly which animation we want and obviously there is nothing to mix, we want 100% of it
      void SetAnimation(const string_t& sAnimation);

      // This is where we want 2 animations at the same time and we specify 1 blending between them
      void SetAnimation(const string_t& sAnimation0, const string_t& sAnimation1);

      // This is where we want 3 animations at the same time and we specify 3 blendings between them
      void SetAnimation(const string_t& sAnimation0, const string_t& sAnimation1, const string_t& sAnimation2);

      // This is for use with 2 animations and we need to know how much of each to use
      void SetAnimationBlendWeight0To1(float_t fBlendWeightFirst0To1);

      // This is for use with 3 animations and we need to know how much of each to use
      void SetAnimationBlendWeight0To1(float_t fBlendWeightFirst0To1, float_t fBlendWeightSecond0To1);

    private:
      size_t animations; // Either 1, 2 or 3 depending on how many there are

      float_t fBlendWeight0;
      float_t fBlendWeight1;
      float_t fBlendWeight2;

      string_t sAnimation0;
      string_t sAnimation1;
      string_t sAnimation2;
    };


    class cAnimation
    {
    public:
      bool LoadFromFile(const string_t& sFullFilename);

      bool IsValid() const { return model.IsValid(); }

      // This is where we know exactly which animation we want and obviously there is nothing to mix, we want 100% of it
      void SetAnimationHead(const string_t& sAnimation) { part[PART_HEAD].SetAnimation(sAnimation); }
      void SetAnimationTorso(const string_t& sAnimation) { part[PART_TORSO].SetAnimation(sAnimation); }
      void SetAnimationLegs(const string_t& sAnimation) { part[PART_LEGS].SetAnimation(sAnimation); }

      // This is where we want 2 animations at the same time and we specify 1 blending between them
      void SetAnimationHead(const string_t& sAnimation0, const string_t& sAnimation1) { part[PART_HEAD].SetAnimation(sAnimation0, sAnimation1); }
      void SetAnimationTorso(const string_t& sAnimation0, const string_t& sAnimation1) { part[PART_TORSO].SetAnimation(sAnimation0, sAnimation1); }
      void SetAnimationLegs(const string_t& sAnimation0, const string_t& sAnimation1) { part[PART_LEGS].SetAnimation(sAnimation0, sAnimation1); }

      // This is where we want 3 animations at the same time and we specify 3 blendings between them
      void SetAnimationHead(const string_t& sAnimation0, const string_t& sAnimation1, const string_t& sAnimation2) { part[PART_HEAD].SetAnimation(sAnimation0, sAnimation1, sAnimation2); }
      void SetAnimationTorso(const string_t& sAnimation0, const string_t& sAnimation1, const string_t& sAnimation2) { part[PART_TORSO].SetAnimation(sAnimation0, sAnimation1, sAnimation2); }
      void SetAnimationLegs(const string_t& sAnimation0, const string_t& sAnimation1, const string_t& sAnimation2) { part[PART_LEGS].SetAnimation(sAnimation0, sAnimation1, sAnimation2); }

      // This is for use with 2 animations and we need to know how much of each to use
      void SetAnimationBlendWeightHead0To1(float_t fBlendWeightFirst0To1) { part[PART_HEAD].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1); }
      void SetAnimationBlendWeightTorso0To1(float_t fBlendWeightFirst0To1) { part[PART_TORSO].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1); }
      void SetAnimationBlendWeightLegs0To1(float_t fBlendWeightFirst0To1) { part[PART_LEGS].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1); }

      // This is for use with 3 animations and we need to know how much of each to use
      void SetAnimationBlendWeightHead0To1(float_t fBlendWeightFirst0To1, float_t fBlendWeightSecond0To1) { part[PART_HEAD].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1, fBlendWeightSecond0To1); }
      void SetAnimationBlendWeightTorso0To1(float_t fBlendWeightFirst0To1, float_t fBlendWeightSecond0To1) { part[PART_TORSO].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1, fBlendWeightSecond0To1); }
      void SetAnimationBlendWeightLegs0To1(float_t fBlendWeightFirst0To1, float_t fBlendWeightSecond0To1) { part[PART_LEGS].SetAnimationBlendWeight0To1(fBlendWeightFirst0To1, fBlendWeightSecond0To1); }

      cMd3 model;

    private:
      static const size_t PART_HEAD = 0;
      static const size_t PART_TORSO = 1;
      static const size_t PART_LEGS = 2;

      cAnimationPart part[3];
    };
  }
}

#endif // CMD3_H
