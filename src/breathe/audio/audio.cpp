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
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <SDL/SDL.h>

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

namespace breathe
{
  namespace audio
  {
    ALCcontext* context = nullptr;
    ALCdevice* device = nullptr;
    ALboolean g_bEAX = false;

    std::map<string_t, cBufferRef> mAudioBuffer;
    typedef std::map<string_t, cBufferRef> ::iterator buffer_iterator;

    std::list<cSourceRef> lAudioSource;
    typedef std::list<cSourceRef>::iterator source_iterator;

    cBufferRef GetAudioBuffer(const string_t& sFilename)
    {
      //return nullptr;

      cBufferRef pBuffer(mAudioBuffer[sFilename]);
      if (pBuffer != nullptr) return pBuffer;

      if (!filesystem::FileExists(sFilename)) {
        SCREEN<<"GetAudioBuffer File not found \""<<sFilename<<"\""<<std::endl;
        return pBuffer;
      }

      pBuffer = cBufferRef(new cBuffer(sFilename));
      ASSERT(pBuffer != nullptr);
      if (!pBuffer->IsValid()) {
        SCREEN<<"GetAudioBuffer Buffer is invalid"<<std::endl;
        //SAFE_DELETE(pBuffer);
        pBuffer.reset();
        return pBuffer;
      }

      mAudioBuffer[sFilename] = pBuffer;

      return pBuffer;
    }

    void AddSource(cSourceRef pSource)
    {
      lAudioSource.push_back(pSource);
    }

    void RemoveSource(cSourceRef pSource)
    {
      lAudioSource.remove(pSource);
    }


    cBufferRef CreateBuffer(const string_t& sFilename)
    {
      return GetAudioBuffer(sFilename);
    }

    void DestroyBuffer(cBufferRef pBuffer)
    {

    }

    cSourceRef CreateSourceAttachedToObject(cBufferRef pBuffer, cObject* pObject)
    {
      ASSERT(pBuffer != nullptr);
      ASSERT(pBuffer->IsValid());

      cSourceRef pSource(new cSource(pBuffer));
      ASSERT(pSource != nullptr);
      ASSERT(pSource->IsValid());

      AddSource(pSource);

      pSource->Attach(pObject);

      return pSource;
    }

    cSourceRef CreateSourceAttachedToScreen(cBufferRef pBuffer)
    {
      ASSERT(pBuffer != nullptr);
      ASSERT(pBuffer->IsValid());

      cSourceRef pSource(new cSource(pBuffer));
      ASSERT(pSource != nullptr);
      ASSERT(pSource->IsValid());

      AddSource(pSource);

      unsigned int source = pSource->GetSource();
      alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
      alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      alSource3f(source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
      alSourcef(source, AL_ROLLOFF_FACTOR, 0.0f);
      alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

      return pSource;
    }

    void DestroySource(cSourceRef pSource)
    {

    }

    void ReportError()
    {
      ALenum error = alGetError();
      switch (error) {
        case (AL_NO_ERROR):
          //SCREEN<<"audio::ReportError AL No error"<<std::endl;
          break;
        case (AL_INVALID_NAME):
          SCREEN<<"audio::ReportError AL Invalid name parameter passed to OpenAL call"<<std::endl;
          break;
        case (AL_INVALID_ENUM):
          SCREEN<<"audio::ReportError AL Invalid enum parameter passed to OpenAL call"<<std::endl;
          break;
        case (AL_INVALID_VALUE):
          SCREEN<<"audio::ReportError AL Invalid parameter value passed to OpenAL call"<<std::endl;
          break;
        case (AL_INVALID_OPERATION):
          SCREEN<<"audio::ReportError AL Illegal call"<<std::endl;
          break;
        case (AL_OUT_OF_MEMORY):
          SCREEN<<"audio::ReportError AL OpenAL is out of memory"<<std::endl;
          break;
        default:
          SCREEN<<"audio::ReportError AL Unknown error code "<<error<<std::endl;
          break;
      }
      ASSERT(error == AL_NO_ERROR);

      error = alcGetError(device);
      switch (error) {
        case ALC_NO_ERROR:
          //SCREEN<<"audio::ReportError ALC No error"<<std::endl;
          break;
        case ALC_INVALID_DEVICE:
          SCREEN<<"audio::ReportError ALC Invalid device parameter passed to OpenAL call"<<std::endl;
          break;
        case ALC_INVALID_CONTEXT:
          SCREEN<<"audio::ReportError ALC Invalid context parameter passed to OpenAL call"<<std::endl;
          break;
        case ALC_INVALID_ENUM:
          SCREEN<<"audio::ReportError ALC Invalid enum parameter passed to OpenAL call"<<std::endl;
          break;
        case ALC_INVALID_VALUE:
          SCREEN<<"audio::ReportError ALC Invalid parameter value passed to OpenAL call"<<std::endl;
          break;
        case ALC_OUT_OF_MEMORY:
          SCREEN<<"audio::ReportError ALC OpenAL is out of memory"<<std::endl;
          break;
        default:
          SCREEN<<"audio::ReportError ALC Unknown error code "<<error<<std::endl;
          //SCREEN<<"audio::ReportError ALC Error "<<error<<":\""<<alcGetErrorString(error)<<"\""<<std::endl;
          break;
      }
      ASSERT(error == ALC_NO_ERROR);

      error = alutGetError();
      switch (error) {
        case (ALUT_ERROR_NO_ERROR):
          //SCREEN<<"audio::ReportError ALUT No error"<<std::endl;
          break;
        case (ALUT_ERROR_INVALID_ENUM):
          SCREEN<<"audio::ReportError ALUT Invalid enum parameter passed to OpenAL call"<<std::endl;
          break;
        case (ALUT_ERROR_INVALID_VALUE):
          SCREEN<<"audio::ReportError ALUT Invalid enum parameter value to OpenAL call"<<std::endl;
          break;
        case (ALUT_ERROR_INVALID_OPERATION):
          SCREEN<<"audio::ReportError ALUT Illegal call"<<std::endl;
          break;
        case (ALUT_ERROR_OUT_OF_MEMORY):
          SCREEN<<"audio::ReportError ALUT OpenAL is out of memory"<<std::endl;
          break;
        default:
          SCREEN<<"audio::ReportError ALUT Error "<<error<<":\""<<alutGetErrorString(error)<<"\""<<std::endl;
          break;
      }
      ASSERT(error == ALUT_ERROR_NO_ERROR);
    }

    void Sleep()
    {
      alutSleep (3);
    }

    void StartAll()
    {
      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      cSourceRef pSource;
      while(iter != iterEnd)
      {
        pSource = (*(iter++));
        if (pSource->IsValid())
          pSource->Play();
      };
    }

    void StopAll()
    {
      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      cSourceRef pSource;
      while(iter != iterEnd)
      {
        pSource = (*(iter++));
        if (pSource->IsValid())
          pSource->Play();
      };
    }

    bool Init()
    {
      LOG<<"Audio"<<std::endl;

      bool bIsEnumerationExtension = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
      ReportError();

      if (bIsEnumerationExtension) {
        LOG<<"audio::Init Extension \"ALC_ENUMERATION_EXT\" is present"<<std::endl;
        const char* szDeviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        ReportError();
        if (szDeviceList == nullptr) LOG<<"audio::Init No devices found"<<std::endl;
        else LOG<<szDeviceList<<std::endl;
      } else LOG<<"audio::Init Extension \"ALC_ENUMERATION_EXT\" is not present"<<std::endl;

#if 1
      LOG<<"Audio alutInitWithoutContext"<<std::endl;
      alutInitWithoutContext(NULL, NULL);
      ReportError();

      // Major minor version number
      ALint iMajor = alutGetMajorVersion();
      ALint iMinor = alutGetMinorVersion();
      std::ostringstream t;
      t<<"OpenAL v";
      t<<iMajor;
      t<<".";
      t<<iMinor;
      LOG<<"audio::Init Audio"<<t.str()<<std::endl;
      ReportError();


#ifdef __WIN__
      const char* szInitString = "DirectSound3D";
#elif defined(__LINUX__)
      const char* szInitString = "'((direction \"write\")) '((devices '(alsa sdl native null)))";
#endif
      LOG<<"audio::Init Opening device \""<<szInitString<<"\""<<std::endl;
      device = alcOpenDevice(szInitString);
      ReportError();
      if (device == NULL) {
        LOG<<"audio::Init alcOpenDevice FAILED"<<std::endl;

        // Ok, that failed, try the default device
        device = alcOpenDevice(NULL); // select the "default device"
        ReportError();
        if (device == NULL) {
          LOG<<"audio::Init alcOpenDevice FAILED, returning"<<std::endl;
          return breathe::BAD;
        }
      }

      // Create our context
      context = alcCreateContext(device, NULL);
      ReportError();
      if (context == NULL) {
        alcCloseDevice(device);
        ReportError();
        LOG<<"audio::Init alcCreateContext FAILED, returning"<<std::endl;
        return breathe::BAD;
      }

      alcMakeContextCurrent(context);
      ReportError();
#else
      alutInit(NULL, NULL);
      ReportError();

      // Major minor version number
      ALint iMajor = alutGetMajorVersion();
      ALint iMinor = alutGetMinorVersion();
      std::ostringstream t;
      t<<"OpenAL v";
      t<<iMajor;
      t<<".";
      t<<iMinor;
      LOG<<"audio::Init Audio"<<t.str()<<std::endl;
      ReportError();

      context = alcGetCurrentContext();
      ReportError();
      if (context == NULL) {
        LOG<<"audio::Init alcGetCurrentContext FAILED, returning"<<std::endl;
        return breathe::BAD;
      }

      // Now get the device from the context
      device = alcGetContextsDevice(context);
      ReportError();
      if (device == NULL) {
        LOG<<"audio::Init alcGetContextsDevice FAILED, returning"<<std::endl;
        return breathe::BAD;
      }
#endif



      // Check for EAX 2.0 support
      LOG<<"audio::Init Checking for EAX 2.0"<<std::endl;
      g_bEAX = alIsExtensionPresent("EAX2.0");
      ReportError();
      printf("EAX is %ssupported\n", (g_bEAX ? "" : "not "));

      /*
        Assuming g_bEAX == AL_TRUE after that code, then EAX 2.0 is available on
        your soundcard.  Note that the reverb is muted by default (Room level ==
        -10000mB), so you will need to change this value (and probably all the
        other reverb parameters to get the reverb effect you want).
      */


      // Surround sound
      LOG<<"audio::Init Checking for surround sound"<<std::endl;
      ALenum eBufferFormat = 0;
#ifndef __LINUX__
      eBufferFormat = alGetEnumValue("AL_FORMAT_81CHN16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "8.1 Surround sound supported, returning");
        return breathe::GOOD;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_71CHN16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "7.1 Surround sound supported, returning");
        return breathe::GOOD;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_61CHN16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "6.1 Surround sound supported, returning");
        return breathe::GOOD;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "5.1 Surround sound supported, returning");
        return breathe::GOOD;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "Quad Speaker Surround sound supported, returning");
        return breathe::GOOD;
      }
#endif

      eBufferFormat = alGetEnumValue("AL_FORMAT_STEREO16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "Stereo sound supported, returning");
        return breathe::GOOD;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_MONO16");
      ReportError();
      if (eBufferFormat) {
        LOG.Success("Audio", "Mono sound supported, returning");
        return breathe::GOOD;
      }

      LOG.Success("Audio", "Unknown sound setup, returning");
      return breathe::BAD;
    }

    void Destroy()
    {
      while (!lAudioSource.empty()) {
        cSourceRef pSource = lAudioSource.back();
        ASSERT(pSource != nullptr);
        pSource.reset();

        lAudioSource.pop_back();
      }

      ASSERT(lAudioSource.empty());

      context = alcGetCurrentContext();
      ReportError();

      device = alcGetContextsDevice(context);
      ReportError();

      alcMakeContextCurrent(NULL);
      ReportError();

      alcDestroyContext(context);
      ReportError();

      if (device != nullptr) {
        alcCloseDevice(device);
        ReportError();
      }

      alutExit();
      ReportError();
    }

    void SetListener(const math::cVec3& position, const math::cVec3& lookat, const math::cVec3& up, const math::cVec3& velocity)
    {
      //LOG<<"SetListener"<<std::endl;
      ALfloat listenerOri[] = { lookat.x, lookat.y, lookat.z, up.x, up.y, up.z };
      alListenerfv(AL_POSITION, position.GetPointerConst());
      //ReportError();
      alListenerfv(AL_VELOCITY, velocity.GetPointerConst());
      //ReportError();
      alListenerfv(AL_ORIENTATION, listenerOri);
      //ReportError();
    }

    void Update(sampletime_t currentTime, const math::cVec3& listenerPosition, const math::cVec3& listenerTarget, const math::cVec3& listenerUp)
    {
      static math::cVec3 positionPrevious = listenerPosition;

      const math::cVec3 listenerVelocity = listenerPosition - positionPrevious;

      SetListener(listenerPosition, listenerTarget, listenerUp, listenerVelocity);

      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      std::list<cSourceRef> listToRemove;

      cSourceRef pSource;
      for (; iter != iterEnd; iter++) {
        pSource = (*(iter));
        if (pSource == nullptr) continue;

        if (pSource->IsPlaying()) {
          pSource->Update();
          continue;
        }

        // Add it to the list of sources to remove, leave the removing until the end
        listToRemove.push_back(pSource);
      };


      iter = listToRemove.begin();
      iterEnd = listToRemove.end();

      for (; iter != iterEnd; iter++) {
        pSource = (*(iter));
        ASSERT(pSource != nullptr);
        pSource->Remove();
        //SAFE_DELETE(pSource);
        pSource.reset();
      }
    }

    void CreateSoundAttachedToScreenPlayAndForget(const string_t& sFilename)
    {
      cBufferRef pBuffer(CreateBuffer(sFilename));
      if (pBuffer == nullptr) {
        SCREEN<<"CreateSoundAttachedToScreenPlayAndForget \""<<sFilename<<"\" pBuffer=NULL, returning"<<std::endl;
        return;
      }
      ASSERT(pBuffer->IsValid());

      cSourceRef pSource(CreateSourceAttachedToScreen(pBuffer));
      if (pSource == nullptr) {
        SCREEN<<"CreateSoundAttachedToScreenPlayAndForget \""<<sFilename<<"\" pSource=NULL, returning"<<std::endl;
        return;
      }
      ASSERT(pSource->IsValid());

      pSource->Play();
    }



    // ********************************************** cBuffer **********************************************

    cBuffer::cBuffer(const string_t& sInFilename) :
      uiBuffer(0),
      sFilename()
    {
      Create(sInFilename);
    }

    cBuffer::~cBuffer()
    {
      LOG<<"cBuffer::~cBuffer"<<std::endl;
      if (uiBuffer != 0) {
        alDeleteBuffers(1, &uiBuffer);
        ReportError();
      }
    }

    void cBuffer::Create(const string_t& sInFilename)
    {
      SCREEN<<"cBuffer::Create \""<<sInFilename<<"\""<<std::endl;
      sFilename = sInFilename;
      uiBuffer = alutCreateBufferFromFile(breathe::string::ToUTF8(sFilename).c_str());
      ReportError();

      if (uiBuffer == 0) SCREEN<<"Audio could not find file \""<<breathe::string::ToUTF8(sFilename)<<"\""<<std::endl;
      else SCREEN<<"Audio found file \""<<breathe::string::ToUTF8(sFilename)<<"\" uiBuffer="<<uiBuffer<<std::endl;
    }


    // ********************************************** cSound **********************************************

    cSource::cSource(cBufferRef pInBuffer) :
      bLooping(false),
      uiSource(0),
      volume(1.0f),
      pBuffer(),
      pNodeParent(nullptr)
    {
      Create(pInBuffer);
    }

    cSource::cSource(cBufferRef pInBuffer, float fVolume) :
      bLooping(false),
      uiSource(0),
      volume(fVolume),
      pBuffer(),
      pNodeParent(nullptr)
    {
      Create(pInBuffer);
    }

    cSource::~cSource()
    {
      SCREEN<<"cSource::~cSource"<<std::endl;
      if (pBuffer != nullptr) {
        SCREEN<<"cSource::~cSource Calling Release"<<std::endl;
        SCREEN<<"cSource::~cSource Setting pBuffer to NULL"<<std::endl;
        //SAFE_DELETE(pBuffer);
      }

      uiSource = 0;
      pNodeParent = nullptr;
      SCREEN<<"cSource::~cSource returning"<<std::endl;
    }

    void cSource::Create(cBufferRef pInBuffer)
    {
      LOG<<"cSource::Create"<<std::endl;
      ASSERT(pBuffer == nullptr);

      ASSERT(pInBuffer != nullptr);
      ASSERT(pInBuffer->IsValid());

      pBuffer = pInBuffer;
      alGenSources(1, &uiSource);
      ReportError();
      alSourcei(uiSource, AL_BUFFER, pInBuffer->uiBuffer);
      ReportError();
    }

    void cSource::SetVolume(float fVolume)
    {
      volume = fVolume;
      if (IsPlaying()) alSourcef(uiSource, AL_GAIN, volume);
    }

    void cSource::SetPitch(float fPitch)
    {
      // Clamp in the range of 0.01f to 2.0f because apparently that is all AL_PITCH is good for
      ASSERT(fPitch > 0.0f);
      ASSERT(fPitch <= 2.0f);
      pitch = math::clamp(fPitch, 0.01f, 2.0f);

      if (IsPlaying()) alSourcef(uiSource, AL_PITCH, pitch);
    }

    void cSource::SetLooping()
    {
      bLooping = true;
      alSourcei(uiSource, AL_LOOPING, AL_TRUE);
    }

    void cSource::SetNonLooping()
    {
      bLooping = false;
      alSourcei(uiSource, AL_LOOPING, AL_FALSE);
    }

    void cSource::Attach(cObject* pInNodeParent)
    {
      pNodeParent = pInNodeParent;
      AddSource(cSourceRef(this));
    }

    void cSource::Remove()
    {
      LOG<<"cSource::Remove"<<std::endl;
      pNodeParent = NULL;

      if (uiSource != 0) {
        LOG<<"cSource::Remove Calling alDeleteSources with uiSource="<<uiSource<<std::endl;
        //FIXME: This needs to be called
        //alDeleteSources(1, &uiSource);
        ReportError();
        uiSource = 0;
      }

      // Now remove it from the list
      //cSourceRef temp(this);
      //RemoveSource(temp);
      LOG<<"cSource::Remove returning"<<std::endl;
    }

    void cSource::Play()
    {
      LOG<<"cSource::Play"<<std::endl;
      alSourcePlay(uiSource);
      ReportError();
    }

    void cSource::Stop()
    {
      LOG<<"cSource::Stop"<<std::endl;
      alSourceStop(uiSource);
      ReportError();
    }

    void cSource::Update()
    {
      if (pNodeParent != nullptr) {
        //LOG<<"cSource::Update"<<std::endl;
        alSourcefv(uiSource, AL_POSITION, pNodeParent->position.GetPointerConst());
        //ReportError();
      }
    }

    bool cSource::IsValid() const
    {
      return (uiSource != 0) && (pBuffer != nullptr);
    }

    bool cSource::IsPlaying() const
    {
      if (!IsValid()) return false;

      // If we have stopped playing this sound remove us from the list
      ALint value = AL_PLAYING;
      //LOG<<"cSource::IsPlaying"<<std::endl;
      alGetSourcei(uiSource, AL_SOURCE_STATE, &value);
      //ReportError();
      return (AL_PLAYING == value);
    }

    void cSource::TransformTo2DSource()
    {
      LOG<<"cSource::TransformTo2DSource"<<std::endl;
      alSourcei(uiSource, AL_SOURCE_RELATIVE, AL_TRUE);
      ReportError();
      alSourcef(uiSource, AL_ROLLOFF_FACTOR, 0.0);
      ReportError();
    }

    void cSource::TransformTo3DSource()
    {
      LOG<<"cSource::TransformTo2DSource"<<std::endl;
      alSourcei (uiSource, AL_SOURCE_RELATIVE, AL_FALSE);
      ReportError();
      alSourcef (uiSource, AL_ROLLOFF_FACTOR, 1.0);
      ReportError();
    }

  /*


      // Load test.wav
      loadWAVFile(" test.wav",&format,&data,&size,&freq,&loop);
      if ((GetError()))
      {
        DisplayALError("alutLoadWAVFile test.wav : ", error);
        alDeleteBuffers(NUM_BUFFERS, g_Buffers);
        return;
      }
      // Copy test.wav data into AL Buffer 0
      alBufferData(g_Buffers[0],format,data,size,freq);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alBufferData buffer 0 : ", error);
      alDeleteBuffers(NUM_BUFFERS, g_Buffers);
      return;
      }
      // Unload test.wav
      unloadWAV(format,data,size,freq);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alutUnloadWAV : ", error);
      return;
      }
      // Generate Sources
      alGenSources(1,source);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alGenSources 1 : ", error);
      return;
      }
      // Attach buffer 0 to source
      alSourcei(source[0], AL_BUFFER, g_Buffers[0]);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alSourcei AL_BUFFER 0 : ", error);
      }
      // Exit
      context=alcGetCurrentContext();
      device = alcGetContextsDevice(context);
      alcMakeContextCurrent(NULL);
      alcDestroyContext(context);
      alcCloseDevice(device);






      // Set Listener
      ALfloat listenerPos[]={0.0,0.0,0.0};
      ALfloat listenerVel[]={0.0,0.0,0.0};
      ALfloat listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0};
      // Position ...
      alListenerfv(AL_POSITION,listenerPos);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alListenerfv POSITION : ", error);
      return;
      }
      // Velocity ...
      alListenerfv(AL_VELOCITY,listenerVel);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alListenerfv VELOCITY : ", error);
      return;
      }
      // Orientation ...
      alListenerfv(AL_ORIENTATION,listenerOri);
      if ((error = alGetError()) != AL_NO_ERROR)
      {
      DisplayALError("alListenerfv ORIENTATION : ", error);
      return;
      }




      // Set Source Properties
      alGetError(); // clear error state
      alSourcef(source[0],AL_PITCH,1.0f);
      if ((error = alGetError()) != AL_NO_ERROR)
      DisplayALError("alSourcef 0 AL_PITCH : \n", error);
      alGetError(); // clear error state
      alSourcef(source[0],AL_GAIN,1.0f);
      if ((error = alGetError()) != AL_NO_ERROR)
      DisplayALError("alSourcef 0 AL_GAIN : \n", error);

      alGetError(); // clear error state
      alSourcefv(source[0],AL_POSITION,source0Pos);
      if ((error = alGetError()) != AL_NO_ERROR)
      DisplayALError("alSourcefv 0 AL_POSITION : \n", error);
      alGetError(); // clear error state
      alSourcefv(source[0],AL_VELOCITY,source0Vel);
      if ((error = alGetError()) != AL_NO_ERROR)
      DisplayALError("alSourcefv 0 AL_VELOCITY : \n", error);
      alGetError(); // clear error state
      alSourcei(source[0],AL_LOOPING,AL_FALSE);
      if ((error = alGetError()) != AL_NO_ERROR)
      DisplayALError("alSourcei 0 AL_LOOPING true: \n", error);
    }*/





    // For mixing two sounds together for a collision mostly
    cSourceMix::cSourceMix(cBufferRef pBuffer0, cBufferRef pBuffer1, float fVolume0, float fVolume1) :
      source0(pBuffer0, fVolume0),
      source1(pBuffer1, fVolume1)
    {

    }

    void cSourceMix::Attach(cObject* pNodeParent)
    {
      source0.Attach(pNodeParent);
      source1.Attach(pNodeParent);
    }

    void cSourceMix::Remove()
    {
      source0.Remove();
      source1.Remove();
    }

    void cSourceMix::Update()
    {
      source0.Update();
      source1.Update();
    }

    void cSourceMix::Play()
    {
      source0.Play();
      source1.Play();
    }

    void cSourceMix::Stop()
    {
      source0.Stop();
      source1.Stop();
    }

    bool cSourceMix::IsValid() const
    {
      return source0.IsValid() && source1.IsValid();
    }

    bool cSourceMix::IsPlaying() const
    {
      return source0.IsPlaying() && source1.IsPlaying();
    }
  }
}
