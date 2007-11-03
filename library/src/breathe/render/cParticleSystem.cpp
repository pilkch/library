// Standard Library Headers
#include <cmath>
#include <cassert>

// STL Headers
#include <list>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>

// OpenGL Headers
#include <GL/Glee.h>

// SDL Headers
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe Headers
#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>

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
		cParticleSystem::cParticleSystem(size_t uiMaxSize, 
			unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax) :

			uiSize(uiMaxSize),
			uiLifeSpanMin(uiInLifeSpanMin),
			uiLifeSpanMax(uiInLifeSpanMax),
			position(0.0f, 0.0f, 0.0f),
			gravity(0.0f, 0.0f, -0.005f),
			spawnVelocity(0.1f, 0.1f, 0.3f)
		{
			size_t i;
			for (i = 0; i < uiSize; i++)
				particles.push_back(cParticle());
		}

		cParticleSystem::~cParticleSystem()
		{
			particles.clear();
		}
		
		void cParticleSystem::Init()
		{
			size_t n = particles.size();
			for (size_t i = 0; i < n; i++)
				InitParticle(i);
		}

		void cParticleSystem::Clear()
		{
			size_t n = particles.size();
			for (size_t i = 0; i < n; i++)
				particles[i].Kill();
		}
		
		void cParticleSystem::InitParticle(unsigned int uiParticle)
		{
			assert(uiParticle < particles.size());
			
			cParticle* pParticle = &particles[uiParticle];
			pParticle->SetLife(math::random(uiLifeSpanMin, uiLifeSpanMax));
			pParticle->p.Set(0.0f, 0.0f, 0.0f);
			pParticle->vel.Set(
				math::randomMinusOneToPlusOne() * spawnVelocity.x, 
				math::randomMinusOneToPlusOne() * spawnVelocity.y, 
				math::randomMinusOneToPlusOne() * spawnVelocity.z);
		}

		void cParticleSystem::Sort()
		{
			size_t n = particles.size();
			for (size_t i = 0; i < n; i++)
				particles[i].SetDepth((position + particles[i].p - pRender->pFrustum->eye).GetLength());

			std::sort(particles.begin(), particles.end(), cParticle::DepthCompare);
		}


		cParticleSystemBillboard::cParticleSystemBillboard(unsigned int uiMaxSize, 
			unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax,
			float fInParticleWidth, float fInParticleHeight) :

			cParticleSystem(uiMaxSize, uiInLifeSpanMin, uiInLifeSpanMax),

			fParticleWidth(fInParticleWidth),
			fParticleHeight(fInParticleHeight),
			pMaterial(NULL)
		{
		}

		void cParticleSystemBillboard::Update(sampletime_t currentTime)
		{
			std::vector<cParticle>::iterator iter = particles.begin();
			std::vector<cParticle>::iterator iterEnd = particles.end();
			unsigned int i = 0;
			while (iter != iterEnd)
			{
				if (iter->IsAlive())
				{
          iter->vel += gravity;
					iter->p += iter->vel;
					iter->DecrementLife();
				}
				else
					InitParticle(i);

				iter++;
				i++;
			}

			Sort();
		}

		unsigned int cParticleSystemBillboard::Render()
		{
			assert(pMaterial);

			pRender->SetMaterial(pMaterial);
      pRender->SetShaderConstant(pMaterial, "width", fParticleWidth);
      pRender->SetShaderConstant(pMaterial, "height", fParticleHeight);

			glPushMatrix();
				glTranslatef(position.x, position.y, position.z);

				glBegin(GL_QUADS);

					unsigned int uiParticlesRendered = 0;
					cParticle* p = &particles[0];
					size_t n = particles.size();
					for (size_t i = 0; i < n; i++, p++)
					{
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
		
		void cParticleSystemBillboard::SetMaterial(material::cMaterial* pInMaterial)
		{
			assert(pInMaterial);
			pMaterial = pInMaterial;
		}




		cParticleSystemMesh::cParticleSystemMesh(unsigned int uiMaxSize,
			unsigned int uiInLifeSpanMin, unsigned int uiInLifeSpanMax) :
			
			cParticleSystem(uiMaxSize, uiInLifeSpanMin, uiInLifeSpanMax),

			pMesh(NULL)
		{
			
		}
		
		void cParticleSystemMesh::InitParticle(unsigned int uiParticle)
		{
			assert(uiParticle < particles.size());
			
			cParticle* pParticle = &particles[uiParticle];
			pParticle->SetLife(math::random(uiLifeSpanMin, uiLifeSpanMax));
			pParticle->p.Set(0.0f, 0.0f, 0.0f);
			pParticle->vel.Set(
				math::randomMinusOneToPlusOne() * spawnVelocity.x, 
				math::randomMinusOneToPlusOne() * spawnVelocity.y, 
				math::randomMinusOneToPlusOne() * spawnVelocity.z);
		}

		void cParticleSystemMesh::SetMesh(model::cMesh* pInMesh)
		{
			assert(pInMesh);
			pMesh = pInMesh;
		}

		void cParticleSystemMesh::Update(sampletime_t currentTime)
		{
			std::vector<cParticle>::iterator iter = particles.begin();
			std::vector<cParticle>::iterator iterEnd = particles.end();
			unsigned int i = 0;
			while (iter != iterEnd)
			{
				if (iter->IsAlive())
				{
          iter->vel += gravity;
					iter->p += iter->vel;
					iter->DecrementLife();
				}
				else
					InitParticle(i);

				iter++;
				i++;
			}

			Sort();
		}

		unsigned int cParticleSystemMesh::Render()
		{
			assert(pMesh != NULL);

			glPushMatrix();
				glTranslatef(position.x, position.y, position.z);

				if (pMesh->pMaterial) pRender->SetMaterial(pMesh->pMaterial);

				unsigned int uiParticlesRendered = 0;
				cParticle* p = &particles[0];
				size_t n = particles.size();
				for (size_t i = 0; i < n; i++, p++)
				{
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
