// Standard libraries
#include <cmath>
#include <cassert>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// Boost includes
#include <boost/smart_ptr.hpp>

// Other libraries
#ifdef BUILD_AUDIO_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif

#ifdef BUILD_AUDIO_SDLMIXER
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif

// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/geometry.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>

#include <breathe/audio/audio.h>
#ifdef BUILD_AUDIO_OPENAL
#include <breathe/audio/audio_openal.h>
#endif
#ifdef BUILD_AUDIO_SDLMIXER
#include <breathe/audio/audio_sdlmixer.h>
#endif

namespace breathe
{
  namespace audio
  {
    // Global variable unfortunately
    cManager* pManager = nullptr;

    bool Init(DRIVER driver)
    {
      LOG<<"audio::Init"<<std::endl;
      ASSERT(pManager == nullptr);

      switch (driver) {
#ifdef BUILD_AUDIO_OPENAL
        case DRIVER::DRIVER_OPENAL: {
          pManager = new openal::cManager;
          break;
        }
#endif
#ifdef BUILD_AUDIO_SDLMIXER
        case DRIVER::DRIVER_SDLMIXER: {
          pManager = new sdlmixer::cManager;
          break;
        }
#endif
        default: {
          LOG<<"audio::Init UNKNOWN driver"<<std::endl;
        }
      }

      LOG<<"audio::Init returning"<<std::endl;
      ASSERT(pManager != nullptr);

      pManager->Init();

      return (pManager != nullptr);
    }

    void Destroy()
    {
      LOG<<"audio::Destroy"<<std::endl;
      ASSERT(pManager != nullptr);

      pManager->Destroy();

      SAFE_DELETE(pManager);
    }

    cManager* GetManager()
    {
      ASSERT(pManager != nullptr);
      return pManager;
    }



    void cManager::Update(sampletime_t currentTime, const math::cVec3& listenerPosition, const math::cVec3& listenerTarget, const math::cVec3& listenerUp)
    {
      const cListener listener(listenerPosition, listenerTarget, listenerUp);


      std::list<cSourceRef>::iterator iter = sources.begin();
      const std::list<cSourceRef>::iterator iterEnd = sources.end();
      while (iter != iterEnd) {
        (*iter)->Update(currentTime, listener);

        iter++;
      }


      // Sort in order of distance from the listener
      sources.sort(cSource::DistanceFromListenerCompare);


      _Update(currentTime, listener);
    }



    cListener::cListener(const math::cVec3& _position, const math::cVec3& _target, const math::cVec3& _up) :
      position(_position),
      target(_target),
      up(_up)
    {
    }




    bool cSource::DistanceFromListenerCompare(const cSourceRef lhs, const cSourceRef rhs)
    {
      // If either of these sources is attached to the screen, that is the highest priority and we return that source
      if (lhs->IsAttachedToScreen()) return true;
      if (rhs->IsAttachedToScreen()) return false;

      // Ok, these are both spatial, closest source wins
      return (lhs->GetDistanceToListenerMeters() < rhs->GetDistanceToListenerMeters());
    }

    void cSource::Update(sampletime_t currentTime, const cListener& listener)
    {
      fDistanceToListenerMeters = (listener.GetPosition() - position).GetLength();

      _Update(currentTime, listener);
    }




/*
    // For mixing two sounds together for a collision mostly
    cSourceMix::cSourceMix(cBufferRef pBuffer0, cBufferRef pBuffer1, float fVolume0, float fVolume1) :
      source0(new cSource(pBuffer0)),
      source1(new cSource(pBuffer1))
    {
      source0->SetVolume(fVolume0);
      source1->SetVolume(fVolume1);
    }

    void cSourceMix::Remove()
    {
      source0->Remove();
      source1->Remove();
    }

    void cSourceMix::Update()
    {
      source0->Update();
      source1->Update();
    }

    void cSourceMix::Play()
    {
      source0->Play();
      source1->Play();
    }

    void cSourceMix::Stop()
    {
      source0->Stop();
      source1->Stop();
    }

    bool cSourceMix::IsValid() const
    {
      return (source0->IsValid() && source1->IsValid());
    }

    bool cSourceMix::IsPlaying() const
    {
      return (source0->IsPlaying() && source1->IsPlaying());
    }*/
  }
}
