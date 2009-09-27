#ifndef AUDIO_SDLMIXER_H
#define AUDIO_SDLMIXER_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>

#include <spitfire/math/cVec3.h>

#include <breathe/audio/audio.h>

// This uses a VERY simplified 3d sound model to give barely acceptable audio.
//
// We do:
// Attenuation
//
// We do not do:
// Left/right balance
// Front/back balance (ie. sources that are in front of the listener sound identical to sources that are behind the listener)
// Echo/reverb/etc. etc.

namespace breathe
{
  namespace sdlmixer
  {
    typedef char sample_t;

    // Forward declaration
    class cSource;
    class cBuffer;
    typedef cSmartPtr<cSource> cSourceRef;
    typedef cSmartPtr<cBuffer> cBufferRef;

    void ReportError();


    class cManager : public audio::cManager
    {
    public:
      cManager();

      void ReportError() const;

    private:
      virtual bool _Init();
      virtual void _Destroy();

      virtual audio::cBufferRef _CreateBuffer(const string_t& sFilename);
      virtual void _DestroyBuffer(audio::cBufferRef pBuffer);

      virtual audio::cSourceRef _CreateSourceAttachedToObject(audio::cBufferRef pBuffer);
      virtual audio::cSourceRef _CreateSourceAttachedToScreen(audio::cBufferRef pBuffer);
      virtual void _DestroySource(audio::cSourceRef pSource);

      virtual void _AddSource(audio::cSourceRef pSource);
      virtual void _RemoveSource(audio::cSourceRef pSource);

      virtual void _CreateSoundAttachedToScreenPlayAndForget(const breathe::string_t& sFilename);

      virtual void _Update(sampletime_t currentTime, const audio::cListener& listener);

      virtual void _StartAll();
      virtual void _StopAll();


      audio::cBufferRef GetAudioBuffer(const string_t& sFilename);
      void SetListener(const audio::cListener& listener);

      math::cVec3 listenerPosition;
      math::cVec3 listenerLookAtPoint;
      math::cVec3 listenerUp;
      math::cVec3 listenerVelocity;
    };

    // Buffer to hold the audio data
    class cBuffer : public audio::cBuffer
    {
    public:
      explicit cBuffer(const string_t& sFilename);
      ~cBuffer();

      Mix_Chunk* pChunk;

    private:
      cBuffer();
      NO_COPY(cBuffer);

      virtual bool _IsValid() const { return (pChunk != nullptr); }

      void Create(const string_t& sFilename);

      string_t sFilename;
    };

    // The sound object (Has a pointer to a buffer that it uses)
    class cSource : public audio::cSource
    {
    public:
      explicit cSource(audio::cBufferRef pBuffer);
      ~cSource();

      int GetChannel() const { return iChannel; }

      float_t GetActualOutputVolume0To1() const;

    private:
      cSource();
      NO_COPY(cSource);

      virtual bool _IsLooping() const { return bLooping; }
      virtual bool _IsValid() const;
      virtual bool _IsPlaying() const;

      virtual void _SetIsAttachedToScreen();

      virtual void _SetPosition(const spitfire::math::cVec3& position);

      virtual void _SetVolume(float fVolume);
      virtual void _SetPitch(float fPitch);
      virtual void _SetLooping();
      virtual void _SetNonLooping();

      virtual void _Update(sampletime_t currentTime, const audio::cListener& listener);

      virtual void _Play();
      virtual void _Stop();

      virtual void _Remove();

      int iChannel;

      bool bLooping;
      float volume;
      float pitch;

      float_t fVolume0To1;
      float_t fDistanceFromListenerInMeters;

      cBufferRef pBuffer;

      void Create(audio::cBufferRef pBuffer);

      float_t GetAttenuation0To1() const;
    };
  }
}

#endif // AUDIO_SDLMIXER_H
