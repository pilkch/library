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

// Other libraries
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <SDL3/SDL.h>

// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
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

#include <breathe/audio/audio.h>
#include <breathe/audio/audio_openal.h>

namespace breathe
{
  namespace openal
  {
    const float fRollOffFactor = 2.0f;
    const float fDopplerFactor = 1.0f;
    const float fDopplerVelocity = 2200.0f;


    void ReportError()
    {
      openal::cManager* pManager = (openal::cManager*)audio::GetManager();
      pManager->ReportError();
    }

    void cManager::ReportError() const
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

      error = alutGetError();
      switch (error) {
        case ALUT_ERROR_NO_ERROR:
          //SCREEN<<"audio::ReportError ALUT No error"<<std::endl;
          break;
        case ALUT_ERROR_INVALID_ENUM:
          SCREEN<<"audio::ReportError ALUT Invalid enum parameter passed to OpenAL call"<<std::endl;
          break;
        case ALUT_ERROR_INVALID_VALUE:
          SCREEN<<"audio::ReportError ALUT Invalid enum parameter value to OpenAL call"<<std::endl;
          break;
        case ALUT_ERROR_INVALID_OPERATION:
          SCREEN<<"audio::ReportError ALUT Illegal call"<<std::endl;
          break;
        case ALUT_ERROR_OUT_OF_MEMORY:
          SCREEN<<"audio::ReportError ALUT OpenAL is out of memory"<<std::endl;
          break;
        default:
          SCREEN<<"audio::ReportError ALUT Unknown error code "<<error<<":\""<<alutGetErrorString(error)<<"\""<<std::endl;
          break;
      }
    }





    cManager::cManager() :
      context(nullptr),
      device(nullptr),
      g_bEAX(false)
    {
    }

    static const size_t indentation = 2;
    static const size_t maxmimumWidth = 79;

    static void PrintChar(int c, size_t* width)
    {
       putchar(c);
       *width = ((c == '\n') ? 0 : ((*width) + 1));
    }

    static void PrintIndent(size_t* width)
    {
       for (size_t i = 0; i < indentation; i++) PrintChar(' ', width);
    }

    static void PrintExtensions(const char *header, char separator, const char *extensions)
    {
      size_t width = 0;
      size_t start = 0;
      size_t end = 0;

      printf("<!> Run - PrintExtensions %s: ", header);
      if (extensions == NULL || extensions[0] == '\0') return;

      PrintIndent(&width);
      while (true) {
        if (extensions[end] == separator || extensions[end] == '\0') {
          if (width + end - start + 2 > maxmimumWidth) {
            PrintChar('\n', &width);
            printf("<!> Run - PrintExtensions %s: ", header);
            PrintIndent(&width);
          }

          while(start < end) {
            PrintChar(extensions[start], &width);
            start++;
          }

          if (extensions[end] == '\0') break;

          start++;
          end++;
          if (extensions[end] == '\0') break;

          PrintChar(',', &width);
          PrintChar(' ', &width);
        }
        end++;
      }
      PrintChar('\n', &width);
    }

    bool cManager::_Init()
    {
      std::cout<<"cManager::_Init"<<std::endl;

#if 1
      device = alcOpenDevice(NULL);
      ReportError();
      ASSERT(device != nullptr);

      context = alcCreateContext(device, NULL);
      ReportError();
      ASSERT(context != nullptr);

      alcMakeContextCurrent(context);
      ReportError();
#endif

      bool bIsEnumerationExtension = (alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE);
      ReportError();

      if (bIsEnumerationExtension) {
        std::cout<<"cManager::_Init Extension \"ALC_ENUMERATION_EXT\" is present"<<std::endl;
        const char* szDeviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        ReportError();
        if (szDeviceList == nullptr) std::cout<<"audio::Init No devices found"<<std::endl;
        else std::cout<<szDeviceList<<std::endl;
      } else std::cout<<"cManager::_Init Extension \"ALC_ENUMERATION_EXT\" is not present"<<std::endl;


#if 1
      std::cout<<"cManager::_Init alutInitWithoutContext"<<std::endl;
      alutInitWithoutContext(nullptr, nullptr);
      ReportError();

      // Major minor version number
      ALint iMajor = alutGetMajorVersion();
      ALint iMinor = alutGetMinorVersion();
      std::ostringstream t;
      t<<"OpenAL v";
      t<<iMajor;
      t<<".";
      t<<iMinor;
      std::cout<<"cManager::_Init Audio "<<t.str()<<std::endl;
      ReportError();


      PrintExtensions("ALC extensions", ' ', alcGetString(device, ALC_EXTENSIONS));
      ReportError();

      std::cout<<"cManager::_Init OpenAL vendor string: "<<alGetString(AL_VENDOR)<<std::endl;
      std::cout<<"cManager::_Init OpenAL renderer string: "<<alGetString(AL_RENDERER)<<std::endl;
      std::cout<<"cManager::_Init OpenAL version string: "<<alGetString(AL_VERSION)<<std::endl;
      PrintExtensions("OpenAL extensions", ' ', alGetString(AL_EXTENSIONS));
      ReportError();


      const ALCchar* szDefaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
      ReportError();
      if (szDefaultDevice != nullptr) {
        std::cout<<"cManager::_Init Default device found "<<szDefaultDevice<<std::endl;
        std::cout<<"cManager::_Init Opening device \""<<szDefaultDevice<<"\""<<std::endl;
        device = alcOpenDevice(szDefaultDevice);
        ReportError();
      }

      if (device == NULL) {
#ifdef __WIN__
        const char* szInitString = "DirectSound3D";
#elif defined(__LINUX__)
        const char* szInitString = nullptr;
#endif
        std::cout<<"cManager::_Init Opening device \""<<szInitString<<"\""<<std::endl;
        device = alcOpenDevice(szInitString);
        ReportError();
        if (device == NULL) {
          std::cout<<"cManager::_Init alcOpenDevice FAILED"<<std::endl;

          // Ok, that failed, try the default device
          device = alcOpenDevice(NULL); // select the "default device"
          ReportError();
          if (device == NULL) {
            std::cout<<"cManager::_Init alcOpenDevice FAILED, returning"<<std::endl;
            return false;
          }
        }
      }

      // Create our context
      std::cout<<"cManager::_Init Creating context"<<std::endl;
      const ALCint attributes[] = {
        //ALC_FREQUENCY, 44100, // The rate of audio playback.
        //ALC_REFRESH, 4096, // The size (in bytes) of each chunk sent to the device.
        //ALC_SYNC, AL_TRUE, // Keep the context in sync?
        0, 0
      };
      context = alcCreateContext(device, attributes);
      ReportError();
      if (context == NULL) {
        alcCloseDevice(device);
        ReportError();
        std::cout<<"cManager::_Init alcCreateContext FAILED, returning"<<std::endl;
        return false;
      }

      std::cout<<"cManager::_Init Making context current"<<std::endl;
      alcMakeContextCurrent(context);
      ReportError();

      std::cout<<"cManager::_Init Setting context to processing"<<std::endl;
      alcProcessContext(context);
      ReportError();
#else
      std::cout<<"cManager::_Init alutInit"<<std::endl;
      alutInit(nullptr, nullptr);
      ReportError();

      // Major minor version number
      ALint iMajor = alutGetMajorVersion();
      ALint iMinor = alutGetMinorVersion();
      std::ostringstream t;
      t<<"OpenAL v";
      t<<iMajor;
      t<<".";
      t<<iMinor;
      std::cout<<"cManager::_Init Audio "<<t.str()<<std::endl;
      ReportError();


      context = alcGetCurrentContext();
      ReportError();
      if (context == NULL) {
        alcCloseDevice(device);
        ReportError();
        std::cout<<"cManager::_Init alcGetCurrentContext FAILED, returning"<<std::endl;
        return breathe::BAD;
      }


      std::cout<<"cManager::_Init Making context current"<<std::endl;
      alcMakeContextCurrent(context);
      ReportError();
#endif


      // Check for EAX 2.0 support
      std::cout<<"cManager::_Init Checking for EAX 2.0"<<std::endl;
      g_bEAX = alIsExtensionPresent("EAX2.0");
      ReportError();
      std::cout<<"cManager::_Init EAX "<<(g_bEAX ? "is" : "is not")<<" supported"<<std::endl;

      // Assuming g_bEAX == AL_TRUE after that code, then EAX 2.0 is available on your soundcard.
      // Note that the reverb is muted by default (Room level == -10000mB), so you will need
      // to change this value (and probably all the other reverb parameters to get the reverb effect you want).


      // Surround sound
      std::cout<<"cManager::_Init Checking for surround sound"<<std::endl;
      ALenum eBufferFormat = 0;

      eBufferFormat = alGetEnumValue("AL_FORMAT_81CHN16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init 8.1 Surround sound supported, returning"<<std::endl;
        return true;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_71CHN16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init 7.1 Surround sound supported, returning"<<std::endl;
        return true;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_61CHN16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init 6.1 Surround sound supported, returning"<<std::endl;
        return true;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init 5.1 Surround sound supported, returning"<<std::endl;
        return true;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init Quad Speaker Surround sound supported, returning"<<std::endl;
        return true;
      }


      eBufferFormat = alGetEnumValue("AL_FORMAT_STEREO16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init Stereo sound supported, returning"<<std::endl;
        return true;
      }

      eBufferFormat = alGetEnumValue("AL_FORMAT_MONO16");
      ReportError();
      if (eBufferFormat) {
        std::cout<<"openal cManager::_Init Mono sound supported, returning"<<std::endl;
        return true;
      }


      // Setup OpenAL parameters
      alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
      alDopplerFactor(fDopplerFactor);
      alDopplerVelocity(fDopplerVelocity);

      std::cout<<"openal cManager::_Init Unknown sound setup, returning"<<std::endl;
      return false;
    }

    void cManager::_Destroy()
    {
      while (!lAudioSource.empty()) {
        audio::cSourceRef pSource = lAudioSource.back();
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

      if (context != nullptr) {
        alcDestroyContext(context);
        context = nullptr;
        ReportError();
      }

      if (device != nullptr) {
        alcCloseDevice(device);
        device = nullptr;
        ReportError();
      }

      alutExit();
      ReportError();
    }

    audio::cBufferRef cManager::GetAudioBuffer(const string_t& sFilename)
    {
      audio::cBufferRef pBuffer(mAudioBuffer[sFilename]);
      if (pBuffer != nullptr) return pBuffer;

      if (!filesystem::FileExists(sFilename)) {
        SCREEN<<"GetAudioBuffer File not found \""<<sFilename<<"\""<<std::endl;
        return pBuffer;
      }

      pBuffer.reset(new openal::cBuffer(sFilename));
      ASSERT(pBuffer != nullptr);
      if (!pBuffer->IsValid()) {
        SCREEN<<"GetAudioBuffer Buffer is invalid"<<std::endl;
        pBuffer.reset();
        return pBuffer;
      }

      mAudioBuffer[sFilename] = pBuffer;

      return pBuffer;
    }



    void cManager::_AddSource(audio::cSourceRef pSource)
    {
      lAudioSource.push_back(pSource);
    }

    void cManager::_RemoveSource(audio::cSourceRef pSource)
    {
      lAudioSource.remove(pSource);
    }


    audio::cBufferRef cManager::_CreateBuffer(const string_t& sFilename)
    {
      return GetAudioBuffer(sFilename);
    }

    void cManager::_DestroyBuffer(audio::cBufferRef pBuffer)
    {

    }

    audio::cSourceRef cManager::_CreateSourceAttachedToObject(audio::cBufferRef pBuffer)
    {
      ASSERT(pBuffer != nullptr);
      ASSERT(pBuffer->IsValid());

      audio::cSourceRef pSource(new openal::cSource(pBuffer));
      ASSERT(pSource != nullptr);
      ASSERT(pSource->IsValid());

      AddSource(pSource);

      return pSource;
    }

    audio::cSourceRef cManager::_CreateSourceAttachedToScreen(audio::cBufferRef pBuffer)
    {
      ASSERT(pBuffer != nullptr);
      ASSERT(pBuffer->IsValid());

      cSourceRef pSource(new openal::cSource(pBuffer));
      ASSERT(pSource != nullptr);
      ASSERT(pSource->IsValid());

      AddSource(pSource);

      pSource->SetIsAttachedToScreen();

      return pSource;
    }

    void cManager::_DestroySource(audio::cSourceRef pSource)
    {

    }

    void cManager::_StartAll()
    {
      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      audio::cSourceRef pSource;
      while(iter != iterEnd) {
        pSource = (*iter);
        if (pSource->IsValid()) pSource->Play();

        iter++;
      };
    }

    void cManager::_StopAll()
    {
      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      cSourceRef pSource;
      while(iter != iterEnd) {
        pSource = std::static_pointer_cast<openal::cSource>(*iter);
        if (pSource->IsValid()) pSource->Play();

        iter++;
      };
    }

    void cManager::SetListener(const audio::cListener& listener)
    {
      //std::cout<<"SetListener"<<std::endl;

      const math::cVec3& position = listener.GetPosition();
      const math::cVec3& lookAtPoint = listener.GetTarget();
      const math::cVec3& up = listener.GetUp();

      listenerPositionPrevious = position;

      const math::cVec3 velocity = position - listenerPositionPrevious;

      alListenerfv(AL_POSITION, position.GetPointerConst());
      //ReportError();
      alListenerfv(AL_VELOCITY, velocity.GetPointerConst());
      //ReportError();
      const ALfloat listenerOri[] = {
        lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
        up.x, up.y, up.z
      };
      alListenerfv(AL_ORIENTATION, listenerOri);
      //ReportError();
    }

    void cManager::_Update(durationms_t currentTime, const audio::cListener& listener)
    {
      // Update listener
      SetListener(listener);


      // Find any sources that need removing (Either NULL or finished playing)
      source_iterator iter = lAudioSource.begin();
      source_iterator iterEnd = lAudioSource.end();

      std::list<audio::cSourceRef> listToRemove;

      audio::cSourceRef pSource;
      for (; iter != iterEnd; iter++) {
        pSource = (*(iter));
        if (pSource == nullptr) continue;

        if (pSource->IsPlaying()) {
          pSource->Update(currentTime, listener);
          continue;
        }

        // Add it to the list of sources to remove, leave the removing until the end
        listToRemove.push_back(pSource);
      };


      // Remove any sources that need removing
      //std::cout<<"Removing "<<listToRemove.size()<<" sources"<<std::endl;

      iter = listToRemove.begin();
      iterEnd = listToRemove.end();
      for (; iter != iterEnd; iter++) {
        pSource = (*(iter));
        ASSERT(pSource != nullptr);
        pSource->Remove();

        source_iterator currentSourcesIter = lAudioSource.begin();
        const source_iterator currentSourcesIterEnd = lAudioSource.end();
        while (currentSourcesIter != currentSourcesIterEnd) {
          if (pSource == *currentSourcesIter) {
            lAudioSource.erase(currentSourcesIter);
            break;
          }

          currentSourcesIter++;
        }
      }


      // Sleep to let other threads do some work
      const uint32_t milliseconds = 10;
      alutSleep(0.001f * float(milliseconds));
    }

    void cManager::_CreateSoundAttachedToScreenPlayAndForget(const string_t& sFilename)
    {
      audio::cBufferRef pBuffer(CreateBuffer(sFilename));
      if (pBuffer == nullptr) {
        SCREEN<<"cManager::_CreateSoundAttachedToScreenPlayAndForget \""<<sFilename<<"\" pBuffer=NULL, returning"<<std::endl;
        return;
      }
      ASSERT(pBuffer->IsValid());

      audio::cSourceRef pSource(CreateSourceAttachedToScreen(pBuffer));
      if (pSource == nullptr) {
        SCREEN<<"cManager::_CreateSoundAttachedToScreenPlayAndForget \""<<sFilename<<"\" pSource=NULL, returning"<<std::endl;
        return;
      }
      ASSERT(pSource->IsValid());

      pSource->Play();
    }



    // ********************************************** cBuffer **********************************************

    cBuffer::cBuffer(const string_t& sInFilename) :
      uiBuffer(0)
    {
      Create(sInFilename);
    }

    cBuffer::~cBuffer()
    {
      std::cout<<"cBuffer::~cBuffer"<<std::endl;
      if (uiBuffer != 0) {
        alDeleteBuffers(1, &uiBuffer);
        ReportError();
      }
    }

    void cBuffer::Create(const string_t& sInFilename)
    {
      SCREEN<<"cBuffer::Create \""<<sInFilename<<"\""<<std::endl;

      ASSERT(uiBuffer == 0);

      sFilename = sInFilename;

#if 0
      uiBuffer = alutCreateBufferFromFile(breathe::string::ToUTF8(sFilename).c_str());
#else
      ALfloat frequency = 100.0f;
      ALfloat phase = 0.0f;
      ALfloat duration = 0.1f;
      uiBuffer = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, frequency, phase, duration);
#endif
      ReportError();

      if (uiBuffer == 0) SCREEN<<"Audio could not find file \""<<breathe::string::ToUTF8(sFilename)<<"\""<<std::endl;
      else SCREEN<<"Audio found file \""<<breathe::string::ToUTF8(sFilename)<<"\" uiBuffer="<<uiBuffer<<std::endl;
    }


    // ********************************************** cSound **********************************************

    cSource::cSource(audio::cBufferRef pInBuffer) :
      bLooping(false),
      uiSource(0),
      volume(1.0f),
      pitch(1.0f)
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
      SCREEN<<"cSource::~cSource returning"<<std::endl;
    }

    void cSource::Create(audio::cBufferRef pInBuffer)
    {
      std::cout<<"cSource::Create"<<std::endl;
      ASSERT(pBuffer == nullptr);

      ASSERT(pInBuffer != nullptr);
      ASSERT(pInBuffer->IsValid());

      pBuffer = std::static_pointer_cast<openal::cBuffer>(pInBuffer);

      alGenSources(1, &uiSource);
      ReportError();

      alSourcei(uiSource, AL_BUFFER, pBuffer->uiBuffer);
      ReportError();

      // Set parameters
      alSourcei(uiSource, AL_SOURCE_RELATIVE, AL_FALSE);
      ReportError();
      alSourcef(uiSource, AL_ROLLOFF_FACTOR, fRollOffFactor);
      ReportError();
    }

    void cSource::_SetVolume(float fVolume)
    {
      volume = fVolume;
      if (IsPlaying()) alSourcef(uiSource, AL_GAIN, 10.0f * volume);
    }

    void cSource::_SetPitch(float fPitch)
    {
      // Clamp in the range of 0.01f to 2.0f because apparently that is all AL_PITCH is good for
      ASSERT(fPitch > 0.0f);
      ASSERT(fPitch <= 2.0f);
      pitch = math::clamp(fPitch, 0.01f, 2.0f);

      if (IsPlaying()) alSourcef(uiSource, AL_PITCH, pitch);
    }

    void cSource::_SetLooping()
    {
      bLooping = true;
      alSourcei(uiSource, AL_LOOPING, AL_TRUE);
    }

    void cSource::_SetNonLooping()
    {
      bLooping = false;
      alSourcei(uiSource, AL_LOOPING, AL_FALSE);
    }

    void cSource::_Remove()
    {
      std::cout<<"cSource::_Remove"<<std::endl;

      if (uiSource != 0) {
        std::cerr<<"cSource::_Remove Calling alDeleteSources with uiSource="<<uiSource<<std::endl;
        //FIXME: This needs to be called
        //alDeleteSources(1, &uiSource);
        ReportError();
        uiSource = 0;
      }

      // Now remove it from the list
      //cSourceRef temp(this);
      //RemoveSource(temp);
      std::cout<<"cSource::_Remove returning"<<std::endl;
    }

    void cSource::_Play()
    {
      std::cout<<"cSource::_Play"<<std::endl;
      alSourcePlay(uiSource);
      ReportError();

      SetVolume(volume);
      SetPitch(pitch);
      if (bLooping) SetLooping();
      else SetNonLooping();
    }

    void cSource::_Stop()
    {
      std::cout<<"cSource::_Stop"<<std::endl;
      alSourceStop(uiSource);
      ReportError();
    }

    void cSource::_Update(durationms_t currentTime, const audio::cListener& listener)
    {
    }

    void cSource::_SetPosition(const spitfire::math::cVec3& position)
    {
      //std::cout<<"cSource::_SetPosition"<<std::endl;
      alSourcefv(uiSource, AL_POSITION, position.GetPointerConst());
      //ReportError();
    }

    bool cSource::_IsValid() const
    {
      return (uiSource != 0) && (pBuffer != nullptr);
    }

    bool cSource::_IsPlaying() const
    {
      if (!IsValid()) return false;

      ALint value = AL_PLAYING;
      //std::cout<<"cSource::_IsPlaying"<<std::endl;
      alGetSourcei(uiSource, AL_SOURCE_STATE, &value);
      //ReportError();
      return (AL_PLAYING == value);
    }

    void cSource::_SetIsAttachedToScreen()
    {
      std::cout<<"cSource::_SetIsAttachedToScreen"<<std::endl;
      alSource3f(uiSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSource3f(uiSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSource3f(uiSource, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSourcef(uiSource, AL_ROLLOFF_FACTOR, 0.0f);
      ReportError();
      alSourcei(uiSource, AL_SOURCE_RELATIVE, AL_TRUE);
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
  }
}
