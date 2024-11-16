#ifndef AUDIO_OPENAL_H
#define AUDIO_OPENAL_H

#include <spitfire/spitfire.h>

#include <spitfire/math/cVec3.h>

#include <breathe/audio/audio.h>

// http://www.devmaster.net/articles/openal-tutorials/
// http://www.devmaster.net/articles/openal-tutorials/lesson8.php

// Buffer: Sound data ready to play
// Listener: Position of the player
// Source: Position of an invisible object that plays a buffer

namespace breathe
{
  namespace openal
  {
    typedef char sample_t;

    // Forward declaration
    class cSource;
    class cBuffer;
    typedef std::shared_ptr<cSource> cSourceRef;
    typedef std::shared_ptr<cBuffer> cBufferRef;

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

      virtual void _Update(durationms_t currentTime, const audio::cListener& listener);

      virtual void _StartAll();
      virtual void _StopAll();


      audio::cBufferRef GetAudioBuffer(const string_t& sFilename);
      void SetListener(const audio::cListener& listener);


      ALCcontext* context;
      ALCdevice* device;
      ALboolean g_bEAX;

      math::cVec3 listenerPositionPrevious;
    };

    // Buffer to hold the audio data
    class cBuffer : public audio::cBuffer
    {
    public:
      explicit cBuffer(const string_t& sFilename);
      ~cBuffer();

      unsigned int uiBuffer;

    private:
      cBuffer();

      // No copy
      cBuffer(const cBuffer&) = delete;
      cBuffer& operator=(const cBuffer&) = delete;
      cBuffer(cBuffer&&) noexcept = delete;
      cBuffer& operator=(cBuffer&&) noexcept = delete;

      virtual bool _IsValid() const { return (uiBuffer != 0); }

      void Create(const string_t& sFilename);

      string_t sFilename;
    };

    // The sound object (Has a pointer to a buffer that it uses)
    class cSource : public audio::cSource
    {
    public:
      explicit cSource(audio::cBufferRef pBuffer);
      ~cSource();

      unsigned int GetSource() const { return uiSource; }

    private:
      cSource();

      // No copy
      cSource(const cSource&) = delete;
      cSource& operator=(const cSource&) = delete;
      cSource(cSource&&) noexcept = delete;
      cSource& operator=(cSource&&) noexcept = delete;

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

      bool bLooping;
      unsigned int uiSource;
      float volume;
      float pitch;

      cBufferRef pBuffer;

      void Create(audio::cBufferRef pBuffer);
    };
  }
}

#endif // AUDIO_OPENAL_H
