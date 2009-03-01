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

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>
#include <spitfire/util/unittest.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

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
    const float cSkySystem::fTimeBetweenUpdates = 1.0f;


    // *** cSkySystem

    cSkySystem::cSkySystem() :
      skyDomeAtmosphereRenderer(*this),
      fTimeLastUpdated(0.0f)
    {
      StartFromCurrentLocalEarthTime();
    }

    cSkySystem::~cSkySystem()
    {
      Clear();
    }

    void cSkySystem::StartFromCurrentLocalEarthTime()
    {
      util::cDateTime datetime;
      ASSERT(datetime.IsValid());

      int hours = datetime.GetHours();
      int minutes = datetime.GetMinutes();
      int seconds = datetime.GetSeconds();

      const int totalSeconds = (hours * 3600) + (minutes * 60) + seconds;

      const float fRatioOfTotalDay = float(totalSeconds) / float(util::cSecondsInADay);
      const float fTimeIncrementBetweenUpdates = 10.0f / fTimeBetweenUpdates;
      StartFromTimeAndIncrement0To1(fRatioOfTotalDay, fTimeIncrementBetweenUpdates);
    }

    void cSkySystem::StartFromTimeAndIncrement0To1(float _fTime0To1, float _fTimeIncrement0To1)
    {
      fDayNightCycleTime0To1 = _fTime0To1;
      fTimeIncrement0To1 = _fTimeIncrement0To1;
    }

    void cSkySystem::Update(sampletime_t currentTime)
    {
      float fCurrentTime = float(currentTime);
      if ((fCurrentTime - fTimeLastUpdated) > fTimeBetweenUpdates) {
        fTimeLastUpdated = fCurrentTime;
        fDayNightCycleTime0To1 += fTimeIncrement0To1;
        if (fDayNightCycleTime0To1 > 1.0f) fDayNightCycleTime0To1 -= 1.0f;

        const int totalSeconds = int(fDayNightCycleTime0To1 * 24.0f * 3600.0f);
        const int hours = totalSeconds / 3600;
        const int minutes = (totalSeconds / 60) + (totalSeconds % 3600);
        const int seconds = totalSeconds % 60;
        LOG<<"cSkySystem::Update time="<<hours<<":"<<minutes<<":"<<seconds<<std::endl;
      }
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

    /*
    <skysystem>
      <sun id="sun" primary="" spherical_coordinates="800000.0, 10.0, 3.0" diameterKM="300000" colour="1.0, 1.0, 0.0"/>
      <moon id="moon" spherical_position="30000.0, 10.0, 4.0" diameterKM="10000" colour="1.0, 1.0, 1.0"/>

      <state time0to1="0.0">
        <gradient height="0.3" start_colour="0.0, 0.0, 1.0"/>
        <gradient height="0.7" colour="0.0, 0.0, 1.0"/>
      </state>
      <state time0to1="0.3">
        <gradient height="0.3" start_colour="0.0, 0.0, 1.0"/>
        <gradient height="0.7" colour="0.0, 0.0, 1.0"/>
      </state>
      ...
    </skysystem>
    */

    void cSkySystemLoader::LoadFromFile(cSkySystem& sky, const string_t& sFilename) const
    {
      sky.Clear();

      xml::document doc;

      xml::cReader reader;
      if (!reader.ReadFromFile(doc, sFilename)) {
          LOG<<"cSkySystemLoader::LoadFromFile \""<<sFilename<<"\" could not be loaded, returning"<<std::endl;
          return;
      }

      xml::cNode::iterator iter(doc);

      if (!iter.IsValid()) return;

      iter.FindChild("skysystem");
      if (!iter.IsValid()) return;

      // Look for sun nodes
      {
        xml::cNode::iterator iterSun(iter);

        iterSun.FindChild("sun");
        while (iterSun.IsValid()) {
          string_t sID;
          math::cSphericalCoordinate spherical_coordinates;
          float_t fDiameterKM;
          math::cColour colour;

          iterSun.GetAttribute("id", sID);

          {
            math::cVec3 values;
            if (iterSun.GetAttribute("spherical_coordinates", values)) {
              spherical_coordinates.SetDistance(values.x);
              spherical_coordinates.SetPitchDegrees(values.y);
              spherical_coordinates.SetRotationZDegrees(values.z);
            }
          }

          iterSun.GetAttribute("diameterKM", fDiameterKM);
          iterSun.GetAttribute("colour", colour);

          sky.AddSun(sID, spherical_coordinates, fDiameterKM, colour);

          iterSun.Next("sun");
        };
      }

      // TODO: Look for planet nodes etc.

      LOG<<"cSkySystemLoader::LoadFromFile \""<<sFilename<<"\" loaded, returning"<<std::endl;
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

    void cSkyDomeAtmosphereRenderer::CreateGeometry()
    {
      ClearDome();

      const float hTile = 1.0f;
      const float vTile = 1.0f;

      const float dtheta = 15.0f;
      const float dphi = 15.0f;

      const float dthetaRadians = math::DegreesToRadians(dtheta);
      const float dphiRadians = math::DegreesToRadians(dphi);

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

        const float phiRadians = math::DegreesToRadians(phi);

        for (size_t theta = 0; theta <= 360 - size_t(dtheta); theta += size_t(dtheta)) {
          const float thetaRadians = math::DegreesToRadians(theta);

          // Calculate the vertex at phi, theta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians) * cosf(thetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians) * sinf(thetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians);

          // Create a vector from the origin to this vertex
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          // Normalize the vector
          mag = (float)sqrt(math::squared(vx) + math::squared(vy) + math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          // Calculate the spherical texture coordinates
          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi+dphi, theta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * cosf(thetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * sinf(thetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians + dphiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(math::squared(vx) + math::squared(vy) + math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz) / math::c2_PI) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi, theta+dtheta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians) * cosf(thetaRadians + dthetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians) * sinf(thetaRadians + dthetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(math::squared(vx) + math::squared(vy) + math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / math::cPI) + 0.5f;
          n++;

          // Calculate the vertex at phi+dphi, theta+dtheta
          pVertices[n].x = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * cosf(thetaRadians + dthetaRadians);
          pVertices[n].y = fAtmosphereRadius * sinf(phiRadians + dphiRadians) * sinf(thetaRadians + dthetaRadians);
          pVertices[n].z = fAtmosphereRadius * cosf(phiRadians + dphiRadians);

          // Calculate the texture coordinates
          vx = pVertices[n].x;
          vy = pVertices[n].y;
          vz = pVertices[n].z;

          mag = (float)sqrt(math::squared(vx) + math::squared(vy) + math::squared(vz));
          vx /= mag;
          vy /= mag;
          vz /= mag;

          pVertices[n].u = hTile * (float)(atan2(vx, vz)/(math::cPI * 2)) + 0.5f;
          pVertices[n].v = vTile * (float)(asinf(vy) / math::cPI) + 0.5f;
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
          colourTop.push_back(math::cColour(1.0f, 1.0f, 0.0f, 1.0f));
          colourBottom.push_back(math::cColour(0.0f, 1.0f, 1.0f, 1.0f));
          colourTop.push_back(math::cColour(0.0f, 1.0f, 1.0f, 1.0f));
          colourBottom.push_back(math::cColour(0.0f, 1.0f, 0.0f, 1.0f));
          colourTop.push_back(math::cColour(0.0f, 1.0f, 0.0f, 1.0f));
          colourBottom.push_back(math::cColour(1.0f, 0.0f, 0.0f, 1.0f));

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

    void cSkyDomeAtmosphereRenderer::CreateParticleSystems()
    {
      string_t sFilename;

      const math::cVec3 position(0.0f, 0.0f, 0.0f);
      const math::cVec3 gravity(0.0f, 0.0f, -1.0f);


      pStarParticleSystem = new render::cParticleSystemCustomBillboard();
      sFilename = TEXT("materials/star_billboard.mat");
      pRender->AddMaterial(sFilename);
      pStarParticleSystem->SetMaterial(pRender->GetMaterial(sFilename));

      pStarParticleSystem->SetPosition(position);
      pStarParticleSystem->SetGravity(gravity);

      pPlanetParticleSystem = new render::cParticleSystemCustomBillboard;
      sFilename = TEXT("materials/planet_billboard.mat");
      pRender->AddMaterial(sFilename);
      pPlanetParticleSystem->SetMaterial(pRender->GetMaterial(sFilename));

      pPlanetParticleSystem->SetPosition(position);
      pPlanetParticleSystem->SetGravity(gravity);



      // Add the primary sun
      {
        render::cParticleCustom particle;

        spitfire::math::cSphericalCoordinate s;

        s.SetDistance(fStarLayerRadius);
        s.SetZenithDegrees(0.0f);
        s.SetRotationZDegrees(90.0f);
        s.SetPitchDegrees(0.0f);

        const spitfire::math::cVec3 position(s.GetEulerPosition());
        particle.p.Set(position);

        particle.fWidth = 180.0f;
        particle.fHeight = particle.fWidth;
        particle.colour.Set(spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), 1.0f);

        particle.SetLife(100);

        pStarParticleSystem->particles.push_back(particle);
      }


      // Add the primary moon
      {
        render::cParticleCustom particle;

        spitfire::math::cSphericalCoordinate s;

        s.SetDistance(fPlanetLayerRadius);
        s.SetZenithDegrees(45.0f);
        s.SetRotationZDegrees(45.0f);
        s.SetPitchDegrees(45.0f);

        const spitfire::math::cVec3 position(s.GetEulerPosition());
        particle.p.Set(position);

        particle.fWidth = 50.0f;
        particle.fHeight = particle.fWidth;
        particle.colour.Set(spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), 1.0f);

        particle.SetLife(100);

        pPlanetParticleSystem->particles.push_back(particle);
      }



      for (size_t i = 0; i < 4000; i++) {
        render::cParticleCustom particle;

        spitfire::math::cSphericalCoordinate s;

        s.SetDistance(fStarLayerRadius);
        s.SetZenithDegrees(spitfire::math::randomf(360.0f));
        s.SetRotationZDegrees(spitfire::math::randomf(360.0f));
        s.SetPitchDegrees(spitfire::math::randomf(360.0f));

        const spitfire::math::cVec3 position(s.GetEulerPosition());
        particle.p.Set(position);

        particle.fWidth = spitfire::math::randomf(4.0f, 6.0f);
        particle.fHeight = particle.fWidth;
        particle.colour.Set(spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), 1.0f);

        particle.SetLife(100);

        pStarParticleSystem->particles.push_back(particle);
      }

      for (size_t i = 0; i < 100; i++) {
        render::cParticleCustom particle;

        spitfire::math::cSphericalCoordinate s;

        s.SetDistance(fPlanetLayerRadius);
        s.SetZenithDegrees(spitfire::math::randomf(360.0f));
        s.SetRotationZDegrees(spitfire::math::randomf(360.0f));
        s.SetPitchDegrees(spitfire::math::randomf(360.0f));

        const spitfire::math::cVec3 position(s.GetEulerPosition());
        particle.p.Set(position);

        particle.fWidth = spitfire::math::randomf(5.0f);
        particle.fHeight = particle.fWidth;
        particle.colour.Set(spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), spitfire::math::randomf(1.0f), 1.0f);

        particle.SetLife(100);

        pPlanetParticleSystem->particles.push_back(particle);
      }
    }


    void cSkyDomeAtmosphereRenderer::Update(sampletime_t currentTime)
    {
      LOG<<"cSkyDomeAtmosphereRenderer::Update"<<std::endl;

      // TODO: This should be called from else where
      sky.Update(currentTime);

      const float_t t = float_t(currentTime) / 60000.0f; //0.125f;

      // Set the position of the sun to match the time of day
      ASSERT(pStarParticleSystem != nullptr);
      ASSERT(!pStarParticleSystem->particles.empty());
      render::cParticleCustom& sun = pStarParticleSystem->particles[0];

      spitfire::math::cSphericalCoordinate s;

      s.SetDistance(fPlanetLayerRadius);
      s.SetZenithDegrees(0.0f);
      s.SetRotationZDegrees(90.0f);
      s.SetPitchDegrees(t * 360.0f);

      sun.p = s.GetEulerPosition();


      spitfire::math::cQuaternion q1;
      spitfire::math::cQuaternion q2;

      // If we are in the second half of the rotation then we want to swap the quaternions and come back the other way
      if (t <= 0.5f) {
        q1.SetFromAxisAngle(spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::DegreesToRadians(0.0f));
        q2.SetFromAxisAngle(spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::DegreesToRadians(179.99999f));
      } else {
        q1.SetFromAxisAngle(spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::DegreesToRadians(180.00001f));
        q2.SetFromAxisAngle(spitfire::math::cVec3(0.0f, 0.0f, 1.0f), spitfire::math::DegreesToRadians(0.0f));
      }

      // 2x because we halved the full rotation into two halves
      rotationSun.Slerp(q1, q2, 2.0f * t);


      GenerateTexture();
      pStarParticleSystem->Update(currentTime);
      pPlanetParticleSystem->Update(currentTime);
    }
  }
}
