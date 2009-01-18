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

      void SetDepth(float depth);
      float GetDepth() const;

    private:
      unsigned int life;
      float depth;
    };

    class cParticleSystem : public cRenderable
    {
    public:
      cParticleSystem(size_t uiMaxSize,
        unsigned int uiLifeSpanMin = DEFAULT_LIFESPANMIN, unsigned int uiLifeSpanMax = DEFAULT_LIFESPANMAX);
      virtual ~cParticleSystem();

      void Sort();

      // Parameters
      size_t uiSize;
      unsigned int uiLifeSpanMin;
      unsigned int uiLifeSpanMax;

      math::cVec3 position;
      math::cVec3 gravity;

      math::cVec3 spawnVelocity;

      std::vector<cParticle> particles;

    protected:
      virtual void InitParticle(size_t uiParticle);

    private:
      void Init();
      void Clear();
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
      cParticleSystemMesh(size_t uiMaxSize,
        unsigned int uiLifeSpanMin = DEFAULT_LIFESPANMIN, unsigned int uiLifeSpanMax = DEFAULT_LIFESPANMAX);

      void Update(sampletime_t currentTime);
      unsigned int Render();

      void SetMesh(model::cMeshRef pMesh);

    private:
      model::cMeshRef pMesh;

      void InitParticle(size_t uiParticle);
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
  }
}

#endif // CPARTICLESYSTEM_H
