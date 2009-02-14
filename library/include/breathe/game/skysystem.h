#ifndef SKY_H
#define SKY_H

#include <spitfire/util/datetime.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cSphericalCoordinate.h>

#include <breathe/breathe.h>

#include <breathe/render/cParticleSystem.h>

namespace breathe
{
  namespace sky
  {
    // Of interest is getxyYValuev
    // http://stellarium.svn.sourceforge.net/viewvc/stellarium/trunk/stellarium/src/modules/Skylight.hpp?revision=3521&view=markup
    // http://stellarium.svn.sourceforge.net/viewvc/stellarium/trunk/stellarium/src/modules/Skylight.cpp?revision=3381&view=markup

    // http://www.cs.utah.edu/vissim/papers/sunsky/code/
    // http://www.jmonkeyengine.com/wiki/doku.php?id=skydome
    // http://stellarium.svn.sourceforge.net/viewvc/stellarium/trunk/stellarium/textures/
    // http://caelum.svn.sourceforge.net/viewvc/caelum/trunk/Caelum/main/resources/
    // http://www.cs.utah.edu/vissim/papers/night/nightTech.pdf
    // http://es.geocities.com/kenchoweb/skydomes_en.pdf
    // http://www.flipcode.com/archives/Sky_Domes.shtml


    class cSkyDome;


    class cSkyDomeAtmosphereRenderer
    {
    public:
      explicit cSkyDomeAtmosphereRenderer(cSkySystem& sky);

      bool IsValid() const { return (pTexture != nullptr); }

      void CreateTexture();
      void CreateGeometry(float fAtmosphereRadius);
      void CreateParticleSystems();

      void Update(sampletime_t currentTime);


      struct cVertex {
        float x, y, z;
        float u, v;
      };

      const cVertex* GetVertices() const { return pVertices; }
      size_t GetNumberOfVertices() const { return nVertices; }

      render::cTextureRef GetTexture() { return pTexture; }

      render::cParticleSystemBillboard* GetStarParticleSystem() { return pStarParticleSystem; }
      render::cParticleSystemMesh* GetPlanetParticleSystem() { return pPlanetParticleSystem; }

    private:
      math::cVec2 GetTextureCoordinateFromPosition(const math::cVec3& position) const;

      void ClearDome();

      void GenerateDome(float fAtmosphereRadius);
      void GenerateTexture();

      cSkySystem& sky;

      cVertex* pVertices;
      size_t nVertices;

      render::cTextureFrameBufferObjectRef pTexture;

      render::cParticleSystemBillboard* pStarParticleSystem;
      render::cParticleSystemMesh* pPlanetParticleSystem;
    };

    inline void cSkyDomeAtmosphereRenderer::ClearDome()
    {
      // Make sure our vertex array is clear
      breathe::SAFE_DELETE(pVertices);

      nVertices = 0;
    }

    inline math::cVec2 cSkyDomeAtmosphereRenderer::GetTextureCoordinateFromPosition(const math::cVec3& position) const
    {
      return math::cVec2(position.x, position.y);
    }

    inline void cSkyDomeAtmosphereRenderer::Update(sampletime_t currentTime)
    {
      GenerateTexture();
      pStarParticleSystem->Update(currentTime);
      pPlanetParticleSystem->Update(currentTime);
    }





    class cStarOrPlanet
    {
    public:
      cStarOrPlanet();

      void SetSphericalCoordinatesKM(const math::cSphericalCoordinate& sphericalCoordinatesKM) { positionKM = sphericalCoordinatesKM; }

      math::cVec3 GetEulerPosition() const { return positionKM.GetEulerPosition(); }

      float_t GetDistanceKM() const { return positionKM.GetDistance(); }
      void SetDistanceKM(float_t fDistanceKM) { positionKM.SetDistance(fDistanceKM); }

      float_t GetRotationZDegrees() const { return positionKM.GetRotationZDegrees(); }
      void SetRotationZDegrees(float_t fRotationZDegrees) { positionKM.SetRotationZDegrees(fRotationZDegrees); }

      // Non-standard, these functions convert to more sensible(?) values
      float_t GetPitchDegrees() const { return positionKM.GetPitchDegrees(); }
      void SetPitchDegrees(float_t fPitchDegrees) { positionKM.SetPitchDegrees(fPitchDegrees); }

      float_t GetDiameterKM() const { return fDiameterKM; }
      void SetDiameterKM(float_t _fDiameterKM) { fDiameterKM = _fDiameterKM; }

      const math::cColour& GetColour() const { return colour; }
      void SetColour(const math::cColour& _colour) { colour = _colour; }

      float_t GetBrightness() const { return (1.0f / positionKM.GetDistance()); }

    private:
      math::cSphericalCoordinate positionKM;

      // Starting point in the sky
      math::cSphericalCoordinate startingPositionKM;

      float_t fDiameterKM;
      math::cColour colour;
    };

    inline cStarOrPlanet::cStarOrPlanet() :
      positionKM(100.0f, 0.0f, 0.0f),
      startingPositionKM(100.0f, 0.0f, 0.0f),
      fDiameterKM(10000.0f),
      colour(1.0f, 0.0f, 0.0f)
    {
    }

    typedef cStarOrPlanet cStar;
    typedef cStarOrPlanet cPlanet;


    class cBirdOrCloud
    {
    public:
      const math::cVec3& GetPosition() const { return position; }
      void SetPosition(const math::cVec3& _position) { position = _position; }

      const math::cQuaternion& GetRotation() const { return rotation; }
      void SetRotation(const math::cQuaternion& _rotation) { rotation = _rotation; }

      float_t GetSpeedMetersPerSecond() const { return fSpeedMPS; }
      void SetSpeedMetresPerSecond(float_t _fSpeedMPS) { fSpeedMPS = _fSpeedMPS; }

    private:
      math::cVec3 position;
      // http://en.wikipedia.org/wiki/Quaternion_rotation
      // http://www.flipcode.com/misc/siggraph2000.shtml
      // http://cgi.cse.unsw.edu.au/~cs9018/cgi-bin/moin.cgi/OutLine
      // http://www.jmonkeyengine.com/jmeforum/index.php?topic=1146.0
      // http://www.arges-systems.com/articles/8/proximity-detection-with-triggers
      // http://www.evl.uic.edu/sjames/cs527/project2.html
      math::cQuaternion rotation;
      float_t fSpeedMPS; // Speed in meters per second
    };

    typedef cBirdOrCloud cBird;
    typedef cBirdOrCloud cCloud;


    class cAircraft
    {
    public:
      const math::cVec3& GetPosition() const { return position; }
      void SetPosition(const math::cVec3& _position) { position = _position; }

      const math::cQuaternion& GetRotation() const { return rotation; }
      void SetRotation(const math::cQuaternion& _rotation) { rotation = _rotation; }

      float_t GetSpeedMetersPerSecond() const { return fSpeedMPS; }
      void SetSpeedMetresPerSecond(float_t _fSpeedMPS) { fSpeedMPS = _fSpeedMPS; }

    private:
      math::cVec3 position;
      // http://en.wikipedia.org/wiki/Quaternion_rotation
      // http://www.flipcode.com/misc/siggraph2000.shtml
      // http://cgi.cse.unsw.edu.au/~cs9018/cgi-bin/moin.cgi/OutLine
      // http://www.jmonkeyengine.com/jmeforum/index.php?topic=1146.0
      // http://www.arges-systems.com/articles/8/proximity-detection-with-triggers
      // http://www.evl.uic.edu/sjames/cs527/project2.html
      math::cQuaternion rotation;
      float_t fSpeedMPS; // Speed in meters per second

      cCircularBuffer<math::cVec3> trail;
    };



    class cSkyState
    {
    public:
      math::cColour skyColour;
      //std::vector<cSun> sun;
    };

    class cWeatherState
    {
    public:
      float_t fLight0To2; // 0..2 * fBrightness of the sun/sky at the moment
      float_t fRain0To1;
      float_t fSnowing0To1;
      float_t fDustStorm0To1;
      float_t fThunder0To1;
      float_t fLightningStorm0To1;
    };

    class cWeatherTransition
    {
    public:
      cWeatherState state;
      float_t fStartsInThisManyMS;
      float_t fDurationMS;
    };

    // All stars have the same texture, the only thing that changes is the perceived diameter and colour
    // All planets have the same texture, the only thing that changes is the perceived diameter and colour
    // Suns and moons are really just planets and are added as such, they get no special treatment

    class cSkySystem
    {
    public:
      cSkySystem();
      ~cSkySystem();

      cSkyDomeAtmosphereRenderer& GetSkyDomeAtmosphereRenderer() { return skyDomeAtmosphereRenderer; }

      void Create();
      void Clear();

      void SetNextWeatherTransition(float_t fStartInThisManyMS, float_t );

      void SetPlanetRadius(float_t fObserverPlanetRadius); // This is for the observing planet, ie. the one you are standing on, usually the Earth
      void SetAtmosphereRadius(float_t fObserverAtmosphereRadius); // This is for the observing planet, ie. the one you are standing on, usually the Earth

      void AddBird(const math::cVec3& position, const math::cQuaternion& rotation);
      void AddCloud(const math::cVec3& position, const math::cQuaternion& rotation, float_t fSpeedKPH);
      void AddAircraft(const math::cVec3& position, const math::cQuaternion& rotation, float_t fSpeedKPH);

      void AddStar(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour);
      void AddPlanet(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour);

      void AddSun(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour) { AddStar(sName, position, fDiameterKM, colour); }
      void AddMoon(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour) { AddPlanet(sName, position, fDiameterKM, colour); }


      void SetInterStellarBodySphericalCoordinates(const string_t& sName, const math::cSphericalCoordinate& sphericalCoordinates);
      void SetInterStellarBodyColour(const string_t& sName, const math::cColour& colour);
      void SetInterStellarBodyDiameterKM(const string_t& sName, float_t fDiameterKM);

      void SetPrimarySun(const string_t& sName) { sPrimarySun = sName; }


      std::map<string_t, cStar*>::const_iterator StarsBegin() const { return stars.begin(); }
      const std::map<string_t, cStar*>::const_iterator StarsEnd() const { return stars.end(); }

      std::map<string_t, cPlanet*>::const_iterator PlanetsBegin() const { return planets.begin(); }
      const std::map<string_t, cPlanet*>::const_iterator PlanetsEnd() const { return planets.end(); }

      render::cParticleSystemBillboard* GetStarParticleSystem() { return skyDomeAtmosphereRenderer.GetStarParticleSystem(); }
      //render::cParticleSystemBillboard* GetPlanetParticleSystem() { return skyDomeAtmosphereRenderer.GetPlanetParticleSystem(); }
      render::cParticleSystemMesh* GetPlanetParticleSystem() { return skyDomeAtmosphereRenderer.GetPlanetParticleSystem(); }

    private:
      cStarOrPlanet* GetInterStellarBody(const string_t& sName) const;

      // This is just for relatively close stuff such as birds, clouds and aircraft
      math::cVec3 GetSphericalPositionFromRelativeLocalPositionForAnObjectWithinTheAtmosphere(const math::cVec3& relativePosition) const;

      cSkyState skyState;
      std::map<float_t, cSkyState> daySkyStates;

      cWeatherState weatherState;
      std::list<cWeatherTransition> weatherTransitions;

      cSkyDomeAtmosphereRenderer skyDomeAtmosphereRenderer;

      float_t fObserverPlanetRadius;
      float_t fObserverAtmosphereRadius;

      std::list<cBird*> birds;
      std::list<cCloud*> clouds;
      std::list<cAircraft*> aircraft;

      string_t sPrimarySun;

      std::map<string_t, cStar*> stars;
      std::map<string_t, cPlanet*> planets;
    };

    inline void cSkySystem::Create()
    {
      skyDomeAtmosphereRenderer.CreateTexture();
      skyDomeAtmosphereRenderer.CreateGeometry(800.0f);
      skyDomeAtmosphereRenderer.CreateParticleSystems();
    }

    inline void cSkySystem::AddStar(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour)
    {
      // Get the old star of the same name if possible
      cStar* pStar = stars[sName];
      if (pStar == nullptr) {
        pStar = new cStar;
        stars[sName] = pStar;
      }

      // Set our parameters
      pStar->SetSphericalCoordinatesKM(position);
      pStar->SetDiameterKM(fDiameterKM);
      pStar->SetColour(colour);
    }

    inline void cSkySystem::AddPlanet(const string_t& sName, const math::cSphericalCoordinate& position, float_t fDiameterKM, const math::cColour& colour)
    {
      // Get the old planet of the same name if possible
      cPlanet* pPlanet = planets[sName];
      if (pPlanet == nullptr) {
        pPlanet = new cPlanet;
        planets[sName] = pPlanet;
      }

      // Set our parameters
      pPlanet->SetSphericalCoordinatesKM(position);
      pPlanet->SetDiameterKM(fDiameterKM);
      pPlanet->SetColour(colour);
    }

    inline void cSkySystem::AddBird(const math::cVec3& position, const math::cQuaternion& rotation)
    {
      const float_t fSpeedKPH = math::random(3.0f, 10.0f);
      const float_t fSpeedMPS = math::km_h_to_m_s(fSpeedKPH);

      cBird* pBird;
      pBird->SetPosition(position);
      pBird->SetRotation(rotation);
      pBird->SetSpeedMetresPerSecond(fSpeedMPS);
      birds.push_back(pBird);
    }

    inline void cSkySystem::AddCloud(const math::cVec3& position, const math::cQuaternion& rotation, float_t fSpeedKPH)
    {
      fSpeedKPH = math::clamp(fSpeedKPH, 0.0f, 6.0f);
      const float_t fSpeedMPS = math::km_h_to_m_s(fSpeedKPH);

      cCloud* pCloud;
      pCloud->SetPosition(position);
      pCloud->SetRotation(rotation);
      pCloud->SetSpeedMetresPerSecond(fSpeedMPS);
      clouds.push_back(pCloud);
    }

    inline void cSkySystem::AddAircraft(const math::cVec3& position, const math::cQuaternion& rotation, float_t fSpeedKPH)
    {
      fSpeedKPH = math::clamp(fSpeedKPH, 250.0f, 800.0f);
      const float_t fSpeedMPS = math::km_h_to_m_s(fSpeedKPH);

      cAircraft* pAircraft;
      pAircraft->SetPosition(position);
      pAircraft->SetRotation(rotation);
      pAircraft->SetSpeedMetresPerSecond(fSpeedMPS);
      aircraft.push_back(pAircraft);
    }

    // t = current_time / next_transition_time
    // skyState.fRain0To1 = mix(skyState.fRain0To1, weatherState.fRain0To1, t);


    // This is not automatic, each application must manually call this or optionally create the skystem programmatically

    class cSkySystemLoader
    {
    public:
      void LoadFromFile(cSkySystem& sky, const string_t& sFilename) const;
    };


    class cFlare
    {
    public:
      float_t distanceFromParent;
      math::cColour colour;
      render::cTextureRef texture;
    };

    class cLensFlare
    {
    public:


    private:
      math::cVec3 position;

      std::vector<cFlare> flares;
    };

    class cTimeOfDay
    {
    public:
    };

    // CurrentTime -> Compute SunPosition
    // SunPosition -> cSkyDomeNodeRender - sky overall brightness, including overcast effects, as well as each sky pixel colour
    class cSkyDome
    {
    public:
      cSkyDome();

      void StartFromCurrentLocalEarthTime();
      void StartFromTimeAndIncrement0To1(float fTime0To1, float fTimeIncrement0To1);

      void Update(float fCurrentTime);

    private:
      float fTimeLastUpdated;
      static const float fTimeBetweenUpdates;

      float fDayNightCycleTime0To1; // 0.0f to 1.0f so we can represent any amount of time, ie. not 24 hours
      float fTimeIncrement0To1; // 0.0f to 1.0f how much to add to fDayNightCycleTime0To1 each update

      std::vector<cTimeOfDay> transitionTimes;
    };

    inline cSkyDome::cSkyDome() :
      fTimeLastUpdated(0.0f)
    {
      StartFromCurrentLocalEarthTime();
    }

    inline void cSkyDome::StartFromCurrentLocalEarthTime()
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

    inline void cSkyDome::StartFromTimeAndIncrement0To1(float _fTime0To1, float _fTimeIncrement0To1)
    {
      fDayNightCycleTime0To1 = _fTime0To1;
      fTimeIncrement0To1 = _fTimeIncrement0To1;
    }

    inline void cSkyDome::Update(float fCurrentTime)
    {
      if (fCurrentTime - fTimeLastUpdated > fTimeBetweenUpdates) {
        fTimeLastUpdated = fCurrentTime;
        fDayNightCycleTime0To1 += fTimeIncrement0To1;
        if (fDayNightCycleTime0To1 > 1.0f) fDayNightCycleTime0To1 -= 1.0f;

        const int totalSeconds = int(fDayNightCycleTime0To1 * 24.0f * 3600.0f);
        const int hours = totalSeconds / 3600;
        const int minutes = (totalSeconds / 60) + (totalSeconds % 3600);
        const int seconds = totalSeconds % 60;
        LOG<<"SkyDome time="<<hours<<":"<<minutes<<":"<<seconds<<std::endl;
      }
    }




    //               cSceneNode
    //                 |
    //            cCamera
    //              |
    //      cSkyDomeNode
    //        |
    // cSkyDomeAtmosphereRenderer
    //

    class cSkyDomeNode : public scenegraph3d::cSceneNode
    {
    public:
      explicit cSkyDomeNode(cSkyDomeAtmosphereRenderer& sky);

      void _Visit(scenegraph3d::cCullVisitor& visitor)
      {
        visitor.Visit(sky);
      }

      void _Visit(scenegraph3d::cUpdateVisitor& visitor)
      {
        visitor.Visit(sky);
      }

    private:
      cSkyDomeAtmosphereRenderer& sky;
    };
  }
}

#endif // SKY_H

