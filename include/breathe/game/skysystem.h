#ifndef SKY_H
#define SKY_H

#include <spitfire/util/datetime.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/math/math.h>
#include <spitfire/math/units.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cSphericalCoordinate.h>

#include <breathe/breathe.h>

#include <breathe/render/cParticleSystem.h>

namespace breathe
{
  namespace sky
  {
    const float_t fAtmosphereRadius = 1600.0f;
    const float_t fStarLayerRadius = 1400.0f; // This is how far out the layer of stars is around us, ie. just on the inside of the atmosphere so that they will be rendered slightly later
    const float_t fPlanetLayerRadius = 1200.0f; // This is how far out the layer of planets is around us, ie. just on the inside of the star so that they will be rendered slightly later

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


    // http://www.flipcode.com/archives/Sky_Domes.shtml

    // Positions of all the sky bodies and clouds are computed.
    // From the position of the sun, all the colors for the bodies, clouds, and frame buffer are computed
    // Frame buffer is cleared with the computed sky color
    // Z buffer writes are disabled
    // Render all sky clouds that are marked as being a star layer (a simple texture with dots for stars)
    // Render all sky bodies
    // Render all sky clouds that are clouds and not stars

    // Cloud layers are blended onto the frame buffer.

    // In the case of sky bodies such as the sun, they are not blended, but required an alpha component so the texture does not overdraw
    // elements of the dome that are not part of the actual sun.

    // The moon has to be rendered in 2 passes. First, from the moon texture, which also has an alpha component, a mask is generated that has alpha
    // values of 1.0 for texels inside the moon and alpha values of 0.0 for texels outside the moon. This mask is rendered onto the dome without blending
    // using the current sky color. This is done to remove any stars that might appear behind the moon. Next, the actual moon texture is blended onto the
    // sky dome. The reason it is blended is because during the day, the moon will show a bit of blue or red hue of the sky.

    // Although not a real sky body, the code supports drawing of flares around the sun. These are done by creating duplicate sky
    // body like the sun, but using a flare texture instead of the sun texture. Flares are blended onto the sky dome.


    class cSkyDomeAtmosphereRenderer
    {
    public:
      explicit cSkyDomeAtmosphereRenderer(cSkySystem& sky);

      bool IsValid() const { return (pTexture != nullptr); }

      void CreateTexture();
      void CreateGeometry();
      void CreateParticleSystems();

      void Update(sampletime_t currentTime);


      spitfire::math::cQuaternion GetRotationSun() const { return rotationSun; }

      struct cVertex {
        float x, y, z;
        float u, v;
      };

      const cVertex* GetVertices() const { return pVertices; }
      size_t GetNumberOfVertices() const { return nVertices; }

      render::cTextureRef GetTexture() { return pTexture; }

      render::cParticleSystemCustomBillboard* GetStarParticleSystem() { return pStarParticleSystem; }
      render::cParticleSystemCustomBillboard* GetPlanetParticleSystem() { return pPlanetParticleSystem; }

    private:
      void ClearDome();

      void GenerateDome(float fAtmosphereRadius);
      void GenerateTexture();

      cSkySystem& sky;

      spitfire::math::cQuaternion rotationSun;

      cVertex* pVertices;
      size_t nVertices;

      render::cTextureFrameBufferObjectRef pTexture;

      render::cParticleSystemCustomBillboard* pStarParticleSystem;
      render::cParticleSystemCustomBillboard* pPlanetParticleSystem;
    };

    inline void cSkyDomeAtmosphereRenderer::ClearDome()
    {
      // Make sure our vertex array is clear
      breathe::SAFE_DELETE(pVertices);

      nVertices = 0;
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






    class cWeatherStateTemplate
    {
    public:
      sampletime_t durationMinimumMS;
      sampletime_t durationMaximumMS;

      math::cColour ambientColour; // This is mixed with the day/night cycle ambientColour

      bool bIsThundering;
      sampletime_t thunderFrequencyMinimumMS;
      sampletime_t thunderFrequencyMaximumMS;
      float_t thunderVolume0To1;

      bool bIsLightning;
      bool bIsLightningAccompaniedByThunder; // This allows lightning to generate a thunder clap as it strikes
      sampletime_t lightningFrequencyMinimumMS;
      sampletime_t lightningFrequencyMaximumMS;

      size_t cloudsMimimum;
      size_t cloudsMaximum;

      bool bIsRaining;
      float_t fRainingAmount0To1;

      bool bIsSnowing;
      float_t fSnowingAmount0To1;

      bool bIsFoggy;
      float_t fFogDistanceMinimumm;
      float_t fFogDistanceMaximumm;
      math::cColour fogColour;

      std::map<string_t, float_t> nextStateAndProbability;
    };

    class cWeatherState
    {
    public:
      void SetFromRatioOfTwoStates(float_t fRatio0, const cWeatherState& state0, float_t fRatio1, const cWeatherState& state1);

      sampletime_t timeThisStateStarts;
      sampletime_t timeThisStateEnds;

      math::cColour ambientColour; // This is mixed with the day/night cycle ambientColour

      bool bIsThundering;
      sampletime_t thunderFrequencyMinimumMS;
      sampletime_t thunderFrequencyMaximumMS;
      float_t thunderVolume0To1;

      bool bIsLightning;
      bool bIsLightningAccompaniedByThunder; // This allows lightning to generate a thunder clap as it strikes
      sampletime_t lightningFrequencyMinimumMS;
      sampletime_t lightningFrequencyMaximumMS;

      size_t cloudsMimimum;
      size_t cloudsMaximum;

      bool bIsRaining;
      float_t fRainingAmount0To1;

      bool bIsSnowing;
      float_t fSnowingAmount0To1;

      bool bIsFoggy;
      float_t fFogDistance;
      math::cColour fogColour;
    };

    inline void cWeatherState::SetFromRatioOfTwoStates(float_t fRatio0, const cWeatherState& state0, float_t fRatio1, const cWeatherState& state1)
    {
      ambientColour = (state0.ambientColour * fRatio0) + (state1.ambientColour * fRatio1);

      bIsThundering = (fRatio0 > 0.9f) ? state0.bIsThundering : state1.bIsThundering;
      thunderFrequencyMinimumMS = (state0.thunderFrequencyMinimumMS * fRatio0) + (state1.thunderFrequencyMinimumMS * fRatio1);
      thunderFrequencyMaximumMS = (state0.thunderFrequencyMaximumMS * fRatio0) + (state1.thunderFrequencyMaximumMS * fRatio1);
      thunderVolume0To1 = (state0.thunderVolume0To1 * fRatio0) + (state1.thunderVolume0To1 * fRatio1);

      bIsLightning = (fRatio0 > 0.9f) ? state0.bIsLightning : state1.bIsLightning;
      bIsLightningAccompaniedByThunder = (fRatio0 > 0.9f) ? state0.bIsLightningAccompaniedByThunder : state1.bIsLightningAccompaniedByThunder;
      lightningFrequencyMinimumMS = (state0.lightningFrequencyMinimumMS * fRatio0) + (state1.lightningFrequencyMinimumMS * fRatio1);
      lightningFrequencyMaximumMS = (state0.lightningFrequencyMaximumMS * fRatio0) + (state1.lightningFrequencyMaximumMS * fRatio1);

      cloudsMimimum = (state0.cloudsMimimum * fRatio0) + (state1.cloudsMimimum * fRatio1);
      cloudsMaximum = (state0.cloudsMaximum * fRatio0) + (state1.cloudsMaximum * fRatio1);

      bIsRaining = (fRatio0 > 0.9f) ? state0.bIsRaining : state1.bIsRaining;
      fRainingAmount0To1 = (state0.fRainingAmount0To1 * fRatio0) + (state1.fRainingAmount0To1 * fRatio1);

      bIsFoggy = (fRatio0 > 0.9f) ? state0.bIsFoggy : state1.bIsFoggy;
      fFogDistance = (state0.fFogDistance * fRatio0) + (state1.fFogDistance * fRatio1);
      fogColour = (state0.fogColour * fRatio0) + (state1.fogColour * fRatio1);
    }


    // ** cWeatherManager

    // At any point in time our state looks like this:
    // states[0] .. currentState .. states[1] .. states[2] ...
    // When currentTime >= states[1].startTime we pop states[0] and make sure that states still has at least 2 states in it.
    // If states.size() < 2 then we add a new random state after it
    //
    // t = current_time / next_transition_time
    // skyState.fRain0To1 = mix(skyState.fRain0To1, weatherState.fRain0To1, t);

    class cWeatherManager
    {
    public:
      bool IsValid() const { return (states.size() >= 2); }

      void SetStatesPaused() { bIsStatesCycling = false; }
      void SetStatesCycling() { bIsStatesCycling = true; }

      void Update(sampletime_t currentTime);

    private:
      const cWeatherState& GetState0() const;
      const cWeatherState& GetState1() const;

      void CalculateRatioOfEachState(sampletime_t currentTime, float_t& fRatio0, float_t& fRatio1) const;

      void PlaySoundThunderNormal();
      void PlaySoundThunderLoud();
      void PlaySoundWindNormal();
      void PlaySoundWindStrong();

      void AddLightningFlash(const math::cVec3& position);
      void AddCloud(const math::cVec3& position, const math::cColour& colour);
      void AddRainbow(const math::cVec3& point0, const math::cVec3& point1);

      void SetFogNone();
      void SetFogDistance();

      // Are we cycling through the states or just staying with the current one?
      // This is useful for a cut scene where it is meant to be raining or showing a level that is always snowing
      bool bIsStatesCycling;

      // This is a default state, if no states are loaded then we default to this one
      cWeatherState defaultState;

      // This is the templates of possible states for random selection
      std::map<string_t, cWeatherStateTemplate> statetemplates;

      // This is the list of future states
      std::list<cWeatherState> states;

      // This is the actual state right now, it is positioned at some point between states[0] and states[1] depending on the time
      cWeatherState currentState;
    };

    inline const cWeatherState& cWeatherManager::GetState0() const
    {
      ASSERT(!states.empty());
      return states.front();
    }

    inline const cWeatherState& cWeatherManager::GetState1() const
    {
      ASSERT(states.size() >= 2);
      std::list<cWeatherState>::const_iterator iter = states.begin();
      const std::list<cWeatherState>::const_iterator iterEnd = states.end();
      ASSERT(iter != iterEnd);
      iter++;

      ASSERT(iter != iterEnd);
      return *iter;
    }

    inline void cWeatherManager::CalculateRatioOfEachState(sampletime_t currentTime, float_t& fRatio0, float_t& fRatio1) const
    {
      ASSERT(IsValid());

      fRatio0 = 1.0f;
      fRatio1 = 0.0f;

      currentTime -= currentState.timeThisStateStarts;

      const sampletime_t duration = (currentState.timeThisStateEnds - currentState.timeThisStateStarts);

      ASSERT(duration != 0);

      const float_t fRatioThroughCurrentState = float_t(currentTime) / float_t(duration);

      const float_t fRatioToStartTransitionAt = 0.75f;

      if (fRatioThroughCurrentState > fRatioToStartTransitionAt) {
        // We are in the transition stage, transition from fRatio0 = 1.0f, fRatio1 = 0.0f to fRatio0 = 0.0f, fRatio1 = 1.0f
        const float_t fRatioThroughTransition = (fRatioThroughCurrentState - fRatioToStartTransitionAt) / (1.0f - fRatioToStartTransitionAt);

        fRatio0 = (1.0f - fRatioThroughTransition);
        fRatio1 = fRatioThroughTransition;
      }
    }

    inline void cWeatherManager::Update(sampletime_t currentTime)
    {
      float_t fRatio0 = 1.0f;
      float_t fRatio1 = 0.0f;
      CalculateRatioOfEachState(currentTime, fRatio0, fRatio0);

      const cWeatherState& state0 = GetState0();
      const cWeatherState& state1 = GetState1();

      currentState.SetFromRatioOfTwoStates(fRatio0, state0, fRatio1, state1);
    }

    class cTimeOfDay
    {
    public:
    };

    // All stars have the same texture, the only thing that changes is the perceived diameter and colour
    // All planets have the same texture, the only thing that changes is the perceived diameter and colour
    // Suns and moons are really just planets and are added as such, they get no special treatment
    //
    // CurrentTime -> Compute SunPosition
    // SunPosition -> cSkyDomeNodeRender - sky overall brightness, including overcast effects, as well as each sky pixel colour
    class cSkySystem
    {
    public:
      cSkySystem();
      ~cSkySystem();

      cSkyDomeAtmosphereRenderer& GetSkyDomeAtmosphereRenderer() { return skyDomeAtmosphereRenderer; }

      void Create();
      void Clear();

      void Update(sampletime_t currentTime);

      void StartFromCurrentLocalEarthTime();
      void StartFromTimeAndIncrement0To1(float fTime0To1, float fTimeIncrement0To1);

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

      render::cParticleSystemCustomBillboard* GetStarParticleSystem() { return skyDomeAtmosphereRenderer.GetStarParticleSystem(); }
      render::cParticleSystemCustomBillboard* GetPlanetParticleSystem() { return skyDomeAtmosphereRenderer.GetPlanetParticleSystem(); }


      const math::cVec3& GetPrimarySunPosition() const { return primarySunPosition; }
      const math::cColour& GetAmbientColour() const { return ambientColour; }

    private:
      cStarOrPlanet* GetInterStellarBody(const string_t& sName) const;

      // This is just for relatively close stuff such as birds, clouds and aircraft
      math::cVec3 GetSphericalPositionFromRelativeLocalPositionForAnObjectWithinTheAtmosphere(const math::cVec3& relativePosition) const;

      cSkyState skyState;
      std::map<float_t, cSkyState> daySkyStates;

      cWeatherManager weatherManager;

      cSkyDomeAtmosphereRenderer skyDomeAtmosphereRenderer;

      float_t fObserverPlanetRadius;
      float_t fObserverAtmosphereRadius;

      std::list<cBird*> birds;
      std::list<cCloud*> clouds;
      std::list<cAircraft*> aircraft;

      string_t sPrimarySun;

      std::map<string_t, cStar*> stars;
      std::map<string_t, cPlanet*> planets;


      // Timing
      float fTimeLastUpdated;
      static const float fTimeBetweenUpdates;

      float fDayNightCycleTime0To1; // 0.0f to 1.0f so we can represent any amount of time, ie. not 24 hours
      float fTimeIncrement0To1; // 0.0f to 1.0f how much to add to fDayNightCycleTime0To1 each update

      std::vector<cTimeOfDay> transitionTimes;

      math::cVec3 primarySunPosition;
      math::cColour ambientColour;
    };

    inline void cSkySystem::Create()
    {
      StartFromCurrentLocalEarthTime();

      skyDomeAtmosphereRenderer.CreateTexture();
      skyDomeAtmosphereRenderer.CreateGeometry();
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


    // This is not automatic, each application must manually call this or optionally create the skystem programmatically

    class cSkySystemLoader
    {
    public:
      void LoadFromFile(cSkySystem& sky, const string_t& sFilename) const;
    };



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
