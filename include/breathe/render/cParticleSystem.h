#ifndef CPARTICLESYSTEM_H
#define CPARTICLESYSTEM_H

namespace breathe
{
  namespace render
  {
    const unsigned int DEFAULT_LIFESPANMIN = 30;
    const unsigned int DEFAULT_LIFESPANMAX = 50;
    const float DEFAULT_PARTICLE_WIDTH = 1.0f;
    const float DEFAULT_PARTICLE_HEIGHT = 1.0f;

    namespace model
    {
      class cMesh;
      typedef cSmartPtr<cMesh> cMeshRef;
    }

    class cParticle
    {
    public:
      cParticle();

      static bool DepthCompare(const cParticle& lhs, const cParticle& rhs);

      math::cVec3 p;
      math::cVec3 vel;

      bool IsAlive() const;

      void SetLife(unsigned int life);
      void DecrementLife();
      void Kill();

      float GetDepth() const;
      void SetDepth(float depth);

    private:
      unsigned int life;
      float depth;
    };

    class cParticleSystem : public cRenderable
    {
    public:
      cParticleSystem(size_t uiMaxSize, unsigned int uiLifeSpanMin = DEFAULT_LIFESPANMIN, unsigned int uiLifeSpanMax = DEFAULT_LIFESPANMAX);
      virtual ~cParticleSystem();

      void Sort();

      void SetPosition(const math::cVec3& _position) { position = _position; }
      void SetGravity(const math::cVec3& _gravity) { gravity = _gravity; }

      // Parameters
      size_t uiSize;
      unsigned int uiLifeSpanMin;
      unsigned int uiLifeSpanMax;

      math::cVec3 spawnVelocity;

      std::vector<cParticle> particles;

    protected:
      void InitParticle(size_t uiParticle) { _InitParticle(uiParticle); }

      math::cVec3 position;
      math::cVec3 gravity;

    private:
      void Init();
      void Clear();

      virtual void _InitParticle(size_t uiParticle);
    };

    // This particle system has a constant number of particles,
    // as soon as a particle dies it is respawned, use it in situations such as
    // waterfalls, fire, car exhausts, smoke trails etc. where you want a constant
    // stream of particles
    class cParticleSystemBillboard : public cParticleSystem
    {
    public:
      cParticleSystemBillboard(size_t uiMaxSize,
        unsigned int uiLifeSpanMin = DEFAULT_LIFESPANMIN, unsigned int uiLifeSpanMax = DEFAULT_LIFESPANMAX,
        float fInParticleWidth = DEFAULT_PARTICLE_WIDTH, float fInParticleHeight = DEFAULT_PARTICLE_HEIGHT);

      void Update(sampletime_t currentTime);
      unsigned int Render();

      void SetParticleWidth(float fWidth);
      void SetParticleHeight(float fHeight);
      void SetMaterial(material::cMaterialRef pMaterial);

    private:
      float fParticleWidth;
      float fParticleHeight;

      material::cMaterialRef pMaterial;
    };

    class cParticleSystemMesh : public cParticleSystem
    {
    public:
      cParticleSystemMesh(size_t uiMaxSize, unsigned int uiLifeSpanMin = DEFAULT_LIFESPANMIN, unsigned int uiLifeSpanMax = DEFAULT_LIFESPANMAX);

      void Update(sampletime_t currentTime);
      unsigned int Render();

      void SetMesh(model::cMeshRef pMesh);

    private:
      void _InitParticle(size_t uiParticle);

      model::cMeshRef pMesh;
    };




    class cParticleCustom
    {
    public:
      cParticleCustom();

      static bool DepthCompare(const cParticleCustom& lhs, const cParticleCustom& rhs);

      math::cVec3 p;
      math::cVec3 vel;
      float fWidth;
      float fHeight;
      math::cColour colour;

      bool IsAlive() const;

      void SetLife(unsigned int life);
      void DecrementLife();
      void Kill();

      float GetDepth() const;
      void SetDepth(float depth);

    private:
      unsigned int life;
      float depth;
    };

    // This is a dodgy class with public members etc. that is very open and needs to be cleaned up
    // Should use private members and virtual functions to hide the details?
    class cParticleSystemCustomBillboard : public cRenderable
    {
    public:
      cParticleSystemCustomBillboard();
      virtual ~cParticleSystemCustomBillboard();

      void Update(sampletime_t currentTime) { Sort(); }
      unsigned int Render();

      void SetPosition(const math::cVec3& _position) { position = _position; }
      void SetGravity(const math::cVec3& _gravity) { gravity = _gravity; }
      void SetMaterial(material::cMaterialRef pMaterial);

      std::vector<cParticleCustom> particles;

    protected:
      math::cVec3 position;
      math::cVec3 gravity;

    private:
      void Sort();
      void Clear();

      material::cMaterialRef pMaterial;
      std::vector<cParticleCustom> sorted;
    };


    // ************************************** Inlines **************************************

    // *** cParticle

    inline cParticle::cParticle() :
      life(0)
    {
    }

    inline void cParticle::SetDepth(float _depth)
    {
      depth = _depth;
    }

    inline float cParticle::GetDepth() const
    {
      return depth;
    }

    inline bool cParticle::IsAlive() const
    {
      return life > 0;
    }

    inline void cParticle::SetLife(unsigned int _life)
    {
      life = _life;
    }

    inline void cParticle::DecrementLife()
    {
      life--;
    }

    inline void cParticle::Kill()
    {
      life = 0;
    }

    // *** Comparison for sorting particles based on depth

    inline bool cParticle::DepthCompare(const cParticle& lhs, const cParticle& rhs)
    {
      return (lhs.GetDepth() > rhs.GetDepth());
    }







    inline cParticleCustom::cParticleCustom() :
      fWidth(1.0f),
      fHeight(1.0f),
      colour(0.5f, 0.5f, 0.5f, 1.0f),
      life(0)
    {
    }

    inline void cParticleCustom::SetDepth(float _depth)
    {
      depth = _depth;
    }

    inline float cParticleCustom::GetDepth() const
    {
      return depth;
    }

    inline bool cParticleCustom::IsAlive() const
    {
      return life > 0;
    }

    inline void cParticleCustom::SetLife(unsigned int _life)
    {
      life = _life;
    }

    inline void cParticleCustom::DecrementLife()
    {
      life--;
    }

    inline void cParticleCustom::Kill()
    {
      life = 0;
    }

    // *** Comparison for sorting particles based on depth

    inline bool cParticleCustom::DepthCompare(const cParticleCustom& lhs, const cParticleCustom& rhs)
    {
      return (lhs.GetDepth() > rhs.GetDepth());
    }
  }
}

#endif // CPARTICLESYSTEM_H
