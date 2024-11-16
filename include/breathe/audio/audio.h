#ifndef AUDIO_H
#define AUDIO_H

// Standard headers
#include <cmath>
#include <list>
#include <map>
#include <memory>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>

#include <spitfire/math/cVec3.h>

// Breathe headers
#include <breathe/breathe.h>

// Buffer: Sound data ready to play
// Listener: Position of the player
// Source: Position of an invisible object that plays a buffer

namespace breathe
{
  namespace audio
  {
    typedef char sample_t;

    inline float AmplitudeTodB(float fAmplitude)
    {
      return 20.0f * log10(fAmplitude);
    }
 
    inline float dBToAmplitude(float fdB)
    {
      return pow(10.0f, fdB / 20.0f);
    }


    // Forward declaration
    class cManager;

    class cListener;
    class cSource;
    class cBuffer;
    typedef std::shared_ptr<cSource> cSourceRef;
    typedef std::shared_ptr<cBuffer> cBufferRef;

    enum class DRIVER {
#ifdef BUILD_AUDIO_OPENAL
      OPENAL2, // NOTE: We would call this OPENAL but that is defined elsewhere (AL/al.h probably) stupid global defines
#endif
#ifdef BUILD_AUDIO_SDLMIXER
      SDLMIXER,
#endif

#ifdef BUILD_AUDIO_OPENAL
      DEFAULT = OPENAL2
#else
      DEFAULT = SDLMIXER
#endif
    };

    cManager* Create(DRIVER driver);
    void Destroy(cManager* pManager);

    // TODO: Eventually we want to remove this
    cManager* GetManager();

    class cManager
    {
    public:
      virtual ~cManager() {}

      bool Init() { return _Init(); }
      void Destroy() { return _Destroy(); }

      cBufferRef CreateBuffer(const string_t& sFilename) { return _CreateBuffer(sFilename); }
      void DestroyBuffer(cBufferRef pBuffer) { _DestroyBuffer(pBuffer); }

      cSourceRef CreateSourceAttachedToObject(cBufferRef pBuffer) { return _CreateSourceAttachedToObject(pBuffer); }
      cSourceRef CreateSourceAttachedToScreen(cBufferRef pBuffer) { return _CreateSourceAttachedToScreen(pBuffer); }
      void DestroySource(cSourceRef pSource) { _DestroySource(pSource); }

      void AddSource(cSourceRef pSource) { _AddSource(pSource); }
      void RemoveSource(cSourceRef pSource) { _RemoveSource(pSource); }

      void CreateSoundAttachedToScreenPlayAndForget(const breathe::string_t& sFilename) { _CreateSoundAttachedToScreenPlayAndForget(sFilename); }

      void Update(durationms_t currentTime, const math::cVec3& listenerPosition, const math::cVec3& listenerTarget, const math::cVec3& listenerUp);

      void StartAll() { _StartAll(); }
      void StopAll() { _StopAll(); }

    protected:
      std::map<string_t, audio::cBufferRef> mAudioBuffer;
      typedef std::map<string_t, audio::cBufferRef> ::iterator buffer_iterator;

      std::list<audio::cSourceRef> lAudioSource;
      typedef std::list<audio::cSourceRef>::iterator source_iterator;

    private:
      virtual bool _Init() = 0;
      virtual void _Destroy() = 0;

      virtual cBufferRef _CreateBuffer(const string_t& sFilename) = 0;
      virtual void _DestroyBuffer(cBufferRef pBuffer) = 0;

      virtual cSourceRef _CreateSourceAttachedToObject(cBufferRef pBuffer) = 0;
      virtual cSourceRef _CreateSourceAttachedToScreen(cBufferRef pBuffer) = 0;
      virtual void _DestroySource(cSourceRef pSource) = 0;

      virtual void _AddSource(cSourceRef pSource) {}
      virtual void _RemoveSource(cSourceRef pSource) {}

      virtual void _CreateSoundAttachedToScreenPlayAndForget(const breathe::string_t& sFilename) {}

      virtual void _Update(durationms_t currentTime, const cListener& listener) = 0;

      virtual void _StartAll() {}
      virtual void _StopAll() {}
    };



    class cListener
    {
    public:
      cListener(const math::cVec3& position, const math::cVec3& target, const math::cVec3& up);

      const math::cVec3& GetPosition() const { return position; }
      const math::cVec3& GetTarget() const { return target; }
      const math::cVec3& GetUp() const { return up; }

    private:
      math::cVec3 position;
      math::cVec3 target; // TODO: Rename this one
      math::cVec3 up;
    };


    // Buffer to hold the audio data
    class cBuffer
    {
    public:
      cBuffer() {}
      virtual ~cBuffer() {}

      bool IsValid() const { return _IsValid(); }

    private:
      // No copy
      cBuffer(const cBuffer&) = delete;
      cBuffer& operator=(const cBuffer&) = delete;
      cBuffer(cBuffer&&) noexcept = delete;
      cBuffer& operator=(cBuffer&&) noexcept = delete;

      virtual bool _IsValid() const = 0;
    };

    // The sound object (Has a pointer to a buffer that it uses)
    class cSource
    {
    public:
      cSource() {}
      virtual ~cSource() {}

      bool IsLooping() const { return _IsLooping(); }
      bool IsValid() const { return _IsValid(); }
      bool IsPlaying() const { return _IsPlaying(); }

      bool IsAttachedToScreen() const { return bIsAttachedToScreen; }
      void SetIsAttachedToScreen() { _SetIsAttachedToScreen(); }

      const math::cVec3& GetPosition() const { ASSERT(!IsAttachedToScreen()); return position; }
      float_t GetDistanceToListenerMeters() const { ASSERT(!IsAttachedToScreen()); return fDistanceFromListenerMeters; }

      void SetPosition(const spitfire::math::cVec3& position) { _SetPosition(position); }

      void SetVolume(float fVolume) { _SetVolume(fVolume); }
      void SetPitch(float fPitch) { _SetPitch(fPitch); }
      void SetLooping() { _SetLooping(); }
      void SetNonLooping() { _SetNonLooping(); }

      void Update(durationms_t currentTime, const cListener& listener);

      void Play() { _Play(); }
      void Stop() { _Stop(); }

      void Remove() { _Remove(); }


      static bool DistanceFromListenerCompare(const cSourceRef lhs, const cSourceRef rhs);


    protected:
      bool bIsAttachedToScreen;

      math::cVec3 position;

      float_t fDistanceFromListenerMeters;

    private:
      // No copy
      cSource(const cSource&) = delete;
      cSource& operator=(const cSource&) = delete;
      cSource(cSource&&) noexcept = delete;
      cSource& operator=(cSource&&) noexcept = delete;

      virtual bool _IsLooping() const = 0;
      virtual bool _IsValid() const = 0;
      virtual bool _IsPlaying() const = 0;

      virtual void _SetIsAttachedToScreen() = 0;

      virtual void _SetPosition(const spitfire::math::cVec3& position) = 0;

      virtual void _SetVolume(float fVolume) = 0;
      virtual void _SetPitch(float fPitch) = 0;
      virtual void _SetLooping() = 0;
      virtual void _SetNonLooping() = 0;

      virtual void _Update(durationms_t currentTime, const cListener& listener) = 0;

      virtual void _Play() = 0;
      virtual void _Stop() = 0;

      virtual void _Remove() = 0;
    };







/*

    class cSourceStream
    {
    public:
      virtual ~cSourceStream() {}
      void Update(durationms_t currentTime) { _Update(currentTime); }

    private:
      virtual void _Update(durationms_t currentTime) = 0;

      cSourceRef source;
    };

    class cSourceLoopedEffect : public cSourceStream
    {
    public:
      void SetPitch(float fPitch);

    private:
      void _Update(durationms_t currentTime);

      float fPitch;
    };

    inline void cSourceLoopedEffect::SetPitch(float _fPitch)
    {
      fPitch = _fPitch;
    }

    inline void cSourceLoopedEffect::_Update(durationms_t currentTime)
    {

    }


    // Very simple wrapper for mixing two sounds together.
    // You'll basically only do this on a collision between say wood and metal to make the collision
    // sound more natural as opposed to sounding like only wood or only steel
    // Volumes are 0.0f-1.0f, this number is multiplied by the normal level to get the actual volume,
    // so numbers greater than 1.0f will give gain, less than 1.0f will soften the noise sound compared to what
    // it sounds like normally
    class cSourceMix
    {
    public:
      cSourceMix(cBufferRef pBuffer0, cBufferRef pBuffer1, float fVolume0, float fVolume1);

      void Remove();
      void Update();

      void Play();
      void Stop();

      bool IsValid() const;
      bool IsPlaying() const;

    private:
      cSourceMix();

      // No copy
      cSourceMix(const cSourceMix&) = delete;
      cSourceMix& operator=(const cSourceMix&) = delete;
      cSourceMix(cSourceMix&&) noexcept = delete;
      cSourceMix& operator=(cSourceMix&&) noexcept = delete;

      cSourceRef source0;
      cSourceRef source1;
    };

    //  Another possible class is
    //  class cSourceMixN
    //  {
    //  public:
    //    AddSource(cBufferRef pBuffer, float fVolume);
    //
    //  private:
    //    std::list<cSourceRef> lSource;
    //  };*/
  }
}

#endif // AUDIO_H
