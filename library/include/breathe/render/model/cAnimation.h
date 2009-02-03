#ifndef CMODEL_ANIMATION_H
#define CMODEL_ANIMATION_H

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cAnimation : public cModel
      {
      public:
        cAnimation();
        ~cAnimation();

        int Load(const string_t& sFilename);
        void Update(sampletime_t currentTime);

        std::vector<cStatic*> vFrame; //A vector of all the statics in this model
      };
    }
  }
}

#endif // CMODEL_ANIMATION_H
