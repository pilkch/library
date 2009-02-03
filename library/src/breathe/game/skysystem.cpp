#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>


#include <GL/GLee.h>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#ifdef BUILD_PHYSICS_3D
#include <ode/ode.h>
#endif


// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cTimer.h>
#include <breathe/util/unittest.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/game/scenegraph.h>
#include <breathe/game/skysystem.h>

const unsigned int uiNodeNameDisplayTime = 100;


namespace breathe
{
  namespace sky
  {
    const float cSkyDome::fTimeBetweenUpdates = 1.0f;


    // *** cSkySystem

    cSkySystem::cSkySystem() :
      skyDomeAtmosphereRenderer(*this)
    {
    }

    cSkySystem::~cSkySystem()
    {
      Clear();
    }

    void cSkySystem::Clear()
    {
      {
        std::list<cBird*>::iterator iter = birds.begin();
        const std::list<cBird*>::iterator iterEnd = birds.end();

        while (iter != iterEnd) {
          delete *iter;
          iter++;
        };

        birds.clear();
      }
      {
        std::list<cCloud*>::iterator iter = clouds.begin();
        const std::list<cCloud*>::iterator iterEnd = clouds.end();

        while (iter != iterEnd) {
          delete *iter;
          iter++;
        };

        clouds.clear();
      }
      {
        std::list<cAircraft*>::iterator iter = aircraft.begin();
        const std::list<cAircraft*>::iterator iterEnd = aircraft.end();

        while (iter != iterEnd) {
          delete *iter;
          iter++;
        };

        aircraft.clear();
      }

      {
        std::map<string_t, cStar*>::iterator iter = stars.begin();
        const std::map<string_t, cStar*>::iterator iterEnd = stars.end();

        while (iter != iterEnd) {
          delete iter->second;
          iter++;
        };

        stars.clear();
      }

      {
        std::map<string_t, cPlanet*>::iterator iter = planets.begin();
        const std::map<string_t, cPlanet*>::iterator iterEnd = planets.end();

        while (iter != iterEnd) {
          delete iter->second;
          iter++;
        };

        stars.clear();
      }
    }

    cStarOrPlanet* cSkySystem::GetInterStellarBody(const string_t& sName) const
    {
      {
        std::map<string_t, cStar*>::const_iterator iter = stars.begin();
        const std::map<string_t, cStar*>::const_iterator iterEnd = stars.end();

        while (iter != iterEnd) {
          if (sName == iter->first) return iter->second;
          iter++;
        };
      }

      {
        std::map<string_t, cPlanet*>::const_iterator iter = planets.begin();
        const std::map<string_t, cPlanet*>::const_iterator iterEnd = planets.end();

        while (iter != iterEnd) {
          if (sName == iter->first) return iter->second;
          iter++;
        };
      }

      return nullptr;
    }

    math::cVec3 cSkySystem::GetSphericalPositionFromRelativeLocalPositionForAnObjectWithinTheAtmosphere(const math::cVec3& relativePosition) const
    {
      const math::cVec2 xy(relativePosition.GetXY());
      const float_t fDistanceFromViewer = xy.GetLength();
      const float_t z = relativePosition.z - (0.0001f * math::squared(fDistanceFromViewer));

      return math::cVec3(relativePosition.x, relativePosition.y, z);
    }


    // *** cSkySystemLoader

    void cSkySystemLoader::LoadFromFile(cSkySystem& sky, const string_t& sFilename) const
    {
      sky.Clear();

      xml::document doc;

      xml::cReader reader;
      if (!reader.ReadFromFile(doc, sFilename)) {
          LOG<<"cSkySystemLoader::LoadFromFile \""<<sFilename<<"\" could not be loaded, returning"<<std::endl;
          return;
      }
    }


    // *** cSkyDomeAtmosphereRenderer

    cSkyDomeAtmosphereRenderer::cSkyDomeAtmosphereRenderer(cSkySystem& _sky) :
      sky(_sky),
      pVertices(nullptr),
      nVertices(0),
      pTexture(new render::cTextureFrameBufferObject())
    {
    }

    void cSkyDomeAtmosphereRenderer::CreateTexture()
    {
      pTexture->Create();
    }

    void cSkyDomeAtmosphereRenderer::CreateGeometry(float fAtmosphereRadius)
    {
      ClearDome();

      const float hTile = 1.0f;
      const float vTile = 1.0f;

      const float dtheta = 15.0f;
      const float dphi = 15.0f;

      const float dthetaRadians = breathe::math::DegreesToRadians(dtheta);
      const float dphiRadians = breathe::math::DegreesToRadians(dphi);

      // Initialize our Vertex array
      nVertices = size_t((360.0f / dtheta) * (90.0f / dphi) * 4.0f);
      pVertices = new cVertex[nVertices];
      memset(pVertices, nVertices * sizeof(cVertex), 0);

      // Used to calculate the UV coordinates
      float vx;
      float vy;
      float vz;
      float mag;

      // Generate the dome
      ASSERT(dphi < 90.0f);
      ASSERT(dtheta < 360.0f);

      size_t n = 0;
      for (size_t phi = 0; phi <= 90 - size_t(dphi); phi += size_t(dphi)) {

        const float phiRadians = breathe::math::DegreesToRadians(phi);

        for (size_t theta = 0; theta <= 360 - size_t(dtheta); theta += size_t(dtheta)) {
          const float thetaRadians = breathe::math::DegreesToRadians(theta);

          // Calculate the vertex at phi, theta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians) * cosf(thetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians) * sinf(thetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians);

          // Create a vector from the origin to this vertex
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          // Normalize the vector
          mag = (float)sqrt(breathe::math::squared(vx) + breathe::math::squared(vy) + breathe::math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          // Calculate the spherical texture coordinates
          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(breathe::math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / breathe::math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi+dphi, theta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * cosf(thetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * sinf(thetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians + dphiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(breathe::math::squared(vx)+breathe::math::squared(vy)+breathe::math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz) / breathe::math::c2_PI) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / breathe::math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi, theta+dtheta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians) * cosf(thetaRadians + dthetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians) * sinf(thetaRadians + dthetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(breathe::math::squared(vx) + breathe::math::squared(vy) + breathe::math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(breathe::math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / breathe::math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi+dphi, theta+dtheta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * cosf(thetaRadians + dthetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * sinf(thetaRadians + dthetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians + dphiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(breathe::math::squared(vx) + breathe::math::squared(vy) + breathe::math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(breathe::math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / breathe::math::cPI) + 0.5f;
          n++;
        }
      }

      // Fix the problem at the seam
      for (size_t i = 0; i < nVertices - 3; i++) {
        if (pVertices[i].u - pVertices[i+1].u > 0.9f) pVertices[i+1].u += 1.0f;

        if (pVertices[i+1].u - pVertices[i].u > 0.9f) pVertices[i].u += 1.0f;

        if (pVertices[i].u - pVertices[i+2].u > 0.9f) pVertices[i+2].u += 1.0f;

        if (pVertices[i+2].u - pVertices[i].u > 0.9f) pVertices[i].u += 1.0f;

        if (pVertices[i+1].u - pVertices[i+2].u > 0.9f) pVertices[i+2].u += 1.0f;

        if (pVertices[i+2].u - pVertices[i+1].u > 0.9f) pVertices[i+1].u += 1.0f;

        if (pVertices[i].v - pVertices[i+1].v > 0.8f) pVertices[i+1].v += 1.0f;

        if (pVertices[i+1].v - pVertices[i].v > 0.8f) pVertices[i].v += 1.0f;

        if (pVertices[i].v - pVertices[i+2].v > 0.8f) pVertices[i+2].v += 1.0f;

        if (pVertices[i+2].v - pVertices[i].v > 0.8f) pVertices[i].v += 1.0f;

        if (pVertices[i+1].v - pVertices[i+2].v > 0.8f) pVertices[i+2].v += 1.0f;

        if (pVertices[i+2].v - pVertices[i+1].v > 0.8f) pVertices[i+1].v += 1.0f;
      }
    }

    // Generate a big texture that looks like this and then just rotate the sphere :
    //
    //         **
    //        ****
    //         **
    //
    void cSkyDomeAtmosphereRenderer::GenerateTexture()
    {
      pRender->BeginRenderToTexture(pTexture);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        pRender->BeginScreenSpaceRendering();

          std::vector<math::cColour> colourTop;
          std::vector<math::cColour> colourBottom;
          colourTop.push_back(math::cColour(1.0f, 0.0f, 0.0f, 1.0f));
          colourBottom.push_back(math::cColour(0.0f, 1.0f, 0.0f, 1.0f));
          colourTop.push_back(math::cColour(0.0f, 1.0f, 0.0f, 1.0f));
          colourBottom.push_back(math::cColour(0.0f, 1.0f, 1.0f, 1.0f));
          colourTop.push_back(math::cColour(0.0f, 1.0f, 1.0f, 1.0f));
          colourBottom.push_back(math::cColour(1.0f, 1.0f, 0.0f, 1.0f));

          const size_t n = colourTop.size();
          ASSERT(n != 0);
          const float_t fHeight = 1.0f / float_t(n);
          for (size_t i = 0; i < n; i++) {
            pRender->RenderScreenSpaceGradientFilledRectangleTopLeftIsAt(0.0f, float_t(i) * fHeight, 1.0f, fHeight, colourTop[i], colourTop[i], colourBottom[i], colourBottom[i]);
          }

          pRender->ClearColour();

        pRender->EndScreenSpaceRendering();

      pRender->EndRenderToTexture(pTexture);
    }
  }
}
