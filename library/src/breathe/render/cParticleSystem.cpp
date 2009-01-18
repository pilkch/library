// Standard Library Headers
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

// Writing to and from text files
#include <iostream>
#include <fstream>
#include <sstream>

// STL Headers
#include <list>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

// Boost includes
#include <boost/shared_ptr.hpp>

// OpenGL Headers
#include <GL/GLee.h>

// SDL Headers
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe Headers
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cSmartPtr.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/util/base.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cParticleSystem.h>

#include <breathe/render/model/cMesh.h>

namespace breathe
{
  namespace render
  {
    cParticleSystem::cParticleSystem(size_t uiMaxSize, unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax) :
      uiSize(uiMaxSize),
      uiLifeSpanMin(uiInLifeSpanMin),
      uiLifeSpanMax(uiInLifeSpanMax),
      position(0.0f, 0.0f, 0.0f),
      gravity(0.0f, 0.0f, -0.005f),
      spawnVelocity(0.1f, 0.1f, 0.3f)
    {
      size_t i;
      for (i = 0; i < uiSize; i++) particles.push_back(cParticle());
    }

    cParticleSystem::~cParticleSystem()
    {
      particles.clear();
    }

    void cParticleSystem::Init()
    {
      size_t n = particles.size();
      for (size_t i = 0; i < n; i++) InitParticle(i);
    }

    void cParticleSystem::Clear()
    {
      size_t n = particles.size();
      for (size_t i = 0; i < n; i++) particles[i].Kill();
    }

    void cParticleSystem::InitParticle(size_t uiParticle)
    {
      assert(uiParticle < particles.size());

      cParticle* pParticle = &particles[uiParticle];
      pParticle->SetLife(math::random(uiLifeSpanMin, uiLifeSpanMax));
      pParticle->p.Set(0.0f, 0.0f, 0.0f);
      pParticle->vel.Set(
        math::randomMinusOneToPlusOne() * spawnVelocity.x,
        math::randomMinusOneToPlusOne() * spawnVelocity.y,
        math::randomMinusOneToPlusOne() * spawnVelocity.z
      );
    }

    void cParticleSystem::Sort()
    {
      size_t n = particles.size();
      for (size_t i = 0; i < n; i++)
        particles[i].SetDepth((position + particles[i].p - pRender->pFrustum->eye).GetLength());

      std::sort(particles.begin(), particles.end(), cParticle::DepthCompare);
    }


    cParticleSystemBillboard::cParticleSystemBillboard(size_t uiMaxSize,
      unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax,
      float fInParticleWidth, float fInParticleHeight) :

      cParticleSystem(uiMaxSize, uiInLifeSpanMin, uiInLifeSpanMax),

      fParticleWidth(fInParticleWidth),
      fParticleHeight(fInParticleHeight)
    {
    }

    void cParticleSystemBillboard::Update(sampletime_t currentTime)
    {
      unsigned int i = 0;

      std::vector<cParticle>::iterator iter = particles.begin();
      const std::vector<cParticle>::iterator iterEnd = particles.end();
      while (iter != iterEnd) {
        if (iter->IsAlive()) {
          iter->vel += gravity;
          iter->p += iter->vel;
          iter->DecrementLife();
        } else InitParticle(i);

        iter++;
        i++;
      }

      Sort();
    }

    unsigned int cParticleSystemBillboard::Render()
    {
      ASSERT(pMaterial);

      pRender->SetMaterial(pMaterial);
      pRender->SetShaderConstant(pMaterial, "width", fParticleWidth);
      pRender->SetShaderConstant(pMaterial, "height", fParticleHeight);

      glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        glBegin(GL_QUADS);

          unsigned int uiParticlesRendered = 0;
          cParticle* p = &particles[0];
          const size_t n = particles.size();
          for (size_t i = 0; i < n; i++, p++) {
            if (!p->IsAlive()) continue;

            glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 0.0f);
            glVertex3f(p->p.x, p->p.y, p->p.z);
            glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 1.0f);
            glVertex3f(p->p.x, p->p.y, p->p.z);
            glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 1.0f);
            glVertex3f(p->p.x, p->p.y, p->p.z);
            glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 0.0f);
            glVertex3f(p->p.x, p->p.y, p->p.z);

            uiParticlesRendered++;
          }

        glEnd();
      glPopMatrix();

      return uiParticlesRendered;
    }

    void cParticleSystemBillboard::SetParticleWidth(float fWidth)
    {
      fParticleWidth = fWidth;
    }

    void cParticleSystemBillboard::SetParticleHeight(float fHeight)
    {
      fParticleHeight = fHeight;
    }

    void cParticleSystemBillboard::SetMaterial(material::cMaterialRef pInMaterial)
    {
      assert(pInMaterial);
      pMaterial = pInMaterial;
    }




    cParticleSystemMesh::cParticleSystemMesh(size_t uiMaxSize,
      unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax) :

      cParticleSystem(uiMaxSize, uiInLifeSpanMin, uiInLifeSpanMax)
    {

    }

    void cParticleSystemMesh::InitParticle(size_t uiParticle)
    {
      ASSERT(uiParticle < particles.size());

      cParticle* pParticle = &particles[uiParticle];
      pParticle->SetLife(math::random(uiLifeSpanMin, uiLifeSpanMax));
      pParticle->p.Set(0.0f, 0.0f, 0.0f);
      pParticle->vel.Set(
        math::randomMinusOneToPlusOne() * spawnVelocity.x,
        math::randomMinusOneToPlusOne() * spawnVelocity.y,
        math::randomMinusOneToPlusOne() * spawnVelocity.z
      );
    }

    void cParticleSystemMesh::SetMesh(model::cMeshRef pInMesh)
    {
      ASSERT(pInMesh != nullptr);
      pMesh = pInMesh;
    }

    void cParticleSystemMesh::Update(sampletime_t currentTime)
    {
      unsigned int i = 0;

      std::vector<cParticle>::iterator iter = particles.begin();
      std::vector<cParticle>::iterator iterEnd = particles.end();
      while (iter != iterEnd) {
        if (iter->IsAlive()) {
          iter->vel += gravity;
          iter->p += iter->vel;
          iter->DecrementLife();
        } else InitParticle(i);

        iter++;
        i++;
      }

      Sort();
    }

    unsigned int cParticleSystemMesh::Render()
    {
      ASSERT(pMesh != nullptr);

      glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        if (pMesh->pMaterial) pRender->SetMaterial(pMesh->pMaterial);

        unsigned int uiParticlesRendered = 0;
        cParticle* p = &particles[0];
        const size_t n = particles.size();
        for (size_t i = 0; i < n; i++, p++) {
          if (!p->IsAlive()) continue;

          glPushMatrix();
            glTranslatef(p->p.x, p->p.y, p->p.z);
            pRender->RenderMesh(pMesh);
          glPopMatrix();

          uiParticlesRendered++;
        }

      glPopMatrix();

      return uiParticlesRendered;
    }
  }
}
