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


#if 0
    // TODO: We should do something like this to manage the channels
    // This is a very naive and approach, it is just a basic idea, do NOT do this in practice

    class cChannel
    {
    public:
      void SetBuffer();
      void SetVolume();
      void SetPitch();
      void SetLooping();
    };

    class cManager
    {
    public:
      virtual void _Update(durationms_t currentTime, const cListener& listener)
      {
        size_t i = 0;
        const size_t n = channels.size();

        iterator iter = lAudioSources.begin();
        const iterator iterEnd = lAudioSources.end();

        while ((iter != iterEnd) && (i < n)) {
          channels[i].SetSource(*iter);

          i++;
          iter++;
        }
      }

    private:
      std::vector<cChannel> channels;
    };
#endif

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

      virtual void _Update(durationms_t currentTime, const audio::cListener& listener);

      virtual void _StartAll();
      virtual void _StopAll();


      audio::cBufferRef GetAudioBuffer(const string_t& sFilename);
      void SetListener(const audio::cListener& listener);

      math::cVec3 listenerPosition;
      math::cVec3 listenerLookAtPoint;
      math::cVec3 listenerUp;
      math::cVec3 listenerVelocity;
      // This stores the available channels, in future we should probably have something more like what is outlined above
      std::list<int> channels;
    };

    // Buffer to hold the audio data
    class cBuffer : public audio::cBuffer
    {
    public:
      cBuffer(const string_t& sFilename);
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

      virtual void _Update(durationms_t currentTime, const audio::cListener& listener);

      virtual void _Play();
      virtual void _Stop();

      virtual void _Remove();

      int iChannel;

      bool bLooping;
      float volume;
      float pitch;

      float_t fVolume0To1;

      cBufferRef pBuffer;

      void Create(audio::cBufferRef pBuffer);

      float_t GetAttenuation0To1() const;
    };
  }
}

#endif // AUDIO_SDLMIXER_H
