#ifndef AUDIO_H
#define AUDIO_H

// http://www.devmaster.net/articles/openal-tutorials/
// http://www.devmaster.net/articles/openal-tutorials/lesson8.php

// Namespace functions: Global audio system create/destroy

// Buffer: Sound data ready to play
// Listener: Position of the player
// Source: Position of an invisible object that plays a buffer

namespace breathe
{
  namespace audio
  {
    const size_t AUDIO_MAX_BUFFERS = 16;

    typedef char sample_t;

    // Forward declaration
    class cSource;
    class cBuffer;
    typedef cSmartPtr<cSource> cSourceRef;
    typedef cSmartPtr<cBuffer> cBufferRef;

    bool Init();
    void Destroy();

    void ReportError();

    void Update(sampletime_t currentTime);

    void Sleep();

    void SetListener(const math::cVec3& position, const math::cVec3& lookat, const math::cVec3& up, const math::cVec3& velocity);

    void Add(cSourceRef pSource);
    void Remove(cSourceRef pSource);

    cBufferRef CreateBuffer(const string_t& sFilename);
    void DestroyBuffer(cBufferRef pBuffer);

    cSourceRef CreateSourceAttachedToObject(cBufferRef pBuffer, cObject* pObject);
    cSourceRef CreateSourceAttachedToScreen(cBufferRef pBuffer);
    void DestroySource(cSourceRef pSource);

    void StartAll();
    void StopAll();


    void CreateSoundAttachedToScreenPlayAndForget(const breathe::string_t& sFilename);

    // Buffer to hold the audio data
    class cBuffer
    {
    public:
      explicit cBuffer(const string_t& sFilename);
      ~cBuffer();

      bool IsValid() const { return uiBuffer != 0; }

      unsigned int uiBuffer;

    private:
      cBuffer();
      NO_COPY(cBuffer);

      void Create(const string_t& sFilename);

      string_t sFilename;
    };

    // The sound object (Has pointer to node that it is attached to and a pointer to a buffer that it uses)
    class cSource
    {
    public:
      explicit cSource(cBufferRef pBuffer);
      cSource(cBufferRef pBuffer, float fVolume);
      ~cSource();

      void Attach(cObject* pNodeParent);
      void Remove();
      void Update();

      void Play();
      void Stop();

      bool IsLooping() const { return bLooping; }
      bool IsValid() const;
      bool IsPlaying() const;

      void TransformTo2DSource();
      void TransformTo3DSource();

      void SetVolume(float fVolume);
      void SetPitch(float fPitch);
      void SetLooping();
      void SetNonLooping();

      unsigned int GetSource() const { return uiSource; }

    private:
      cSource();
      NO_COPY(cSource);

      bool bLooping;
      unsigned int uiSource;
      float volume;
      float pitch;

      cBufferRef pBuffer;
      cObject* pNodeParent;

      void Create(cBufferRef pBuffer);
    };


    class cSourceStream
    {
    public:
      virtual ~cSourceStream() {}
      void Update(sampletime_t currentTime) { _Update(currentTime); }

    private:
      virtual void _Update(sampletime_t currentTime) = 0;

      cSource source;
    };

    class cSourceLoopedEffect : public cSourceStream
    {
    public:
      void SetPitch(float fPitch);

    private:
      void _Update(sampletime_t currentTime);

      float fPitch;
    };

    inline void cSourceLoopedEffect::SetPitch(float _fPitch)
    {
      fPitch = _fPitch;
    }

    inline void cSourceLoopedEffect::_Update(sampletime_t currentTime)
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

      void Attach(cObject* pNodeParent);
      void Remove();
      void Update();

      void Play();
      void Stop();

      bool IsValid() const;
      bool IsPlaying() const;

    private:
      cSourceMix();
      NO_COPY(cSourceMix);

      cSource source0;
      cSource source1;
    };

    //  Another possible class is
    //  class cSourceMixN
    //  {
    //  public:
    //    AddSource(cBufferRef pBuffer, float fVolume);
    //
    //  private:
    //    std::list<cSourceRef> lSource;
    //  };
  }
}

#endif // AUDIO_H
