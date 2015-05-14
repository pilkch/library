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
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
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
#include <breathe/audio/audio_sdlmixer.h>

namespace breathe
{
  namespace sdlmixer
  {
    const float fRollOffFactor = 2.0f;
    const float fDopplerFactor = 1.0f;
    const float fDopplerVelocity = 2200.0f;



    void ReportError()
    {
      sdlmixer::cManager* pManager = (sdlmixer::cManager*)audio::GetManager();
      pManager->ReportError();
    }



    void cManager::ReportError() const
    {
      const char* szError = Mix_GetError();
      if (szError != nullptr) SCREEN<<"cManager::ReportError \""<<szError<<"\""<<std::endl;
    }



    cManager::cManager()
    {
    }

    bool cManager::_Init()
    {
      LOG("");

      if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        LOG("SDL_InitSubSystem FAILED to init SDL_INIT_AUDIO, returning false");
        ReportError();
        ASSERT(false);
        return false;
      }

      int audio_rate = 22050;
      Uint16 audio_format = AUDIO_S16;
      int audio_channels = 2;
      int audio_buffers = 4096;
      if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
        LOG("Mix_OpenAudio FAILED Unable to open audio, returning false");
        ReportError();
        ASSERT(false);
        return false;
      }

      // Allocate 16 mixing channels
      Mix_AllocateChannels(16);
      ReportError();

      LOG("return true");
      return true;
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


      Mix_CloseAudio();


      SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }


    audio::cBufferRef cManager::GetAudioBuffer(const string_t& sFilename)
    {
      //return nullptr;

      audio::cBufferRef pBuffer(mAudioBuffer[sFilename]);
      if (pBuffer != nullptr) return pBuffer;

      if (!filesystem::FileExists(sFilename)) {
        SCREEN<<TEXT("GetAudioBuffer File not found \"")<<sFilename<<TEXT("\"")<<std::endl;
        return pBuffer;
      }

      pBuffer.reset(new sdlmixer::cBuffer(sFilename));
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

      audio::cSourceRef pSource(new sdlmixer::cSource(pBuffer));
      ASSERT(pSource != nullptr);
      ASSERT(pSource->IsValid());

      AddSource(pSource);

      return pSource;
    }

    audio::cSourceRef cManager::_CreateSourceAttachedToScreen(audio::cBufferRef pBuffer)
    {
      ASSERT(pBuffer != nullptr);
      ASSERT(pBuffer->IsValid());

      cSourceRef pSource(new sdlmixer::cSource(pBuffer));
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
        pSource = boost::static_pointer_cast<sdlmixer::cSource>(*iter);
        if (pSource->IsValid()) pSource->Play();

        iter++;
      };
    }

    void cManager::SetListener(const audio::cListener& listener)
    {
      //LOG("");

      const math::cVec3 previousPosition = listenerPosition;

      listenerPosition = listener.GetPosition();
      listenerLookAtPoint = listener.GetTarget();
      listenerUp = listener.GetUp();
      listenerVelocity = listenerPosition - previousPosition;
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
      //LOG("Removing ", listToRemove.size(), " sources");

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
      pChunk(nullptr)
    {
      Create(sInFilename);
    }

    cBuffer::~cBuffer()
    {
      LOG("");

      // TODO: We should make sure we are not playing this chunk when we free it
      if (pChunk != nullptr) {
        Mix_FreeChunk(pChunk);
        pChunk = nullptr;
      }
    }

    void cBuffer::Create(const string_t& sInFilename)
    {
      SCREEN<<"cBuffer::Create \""<<sInFilename<<"\""<<std::endl;

      ASSERT(pChunk == nullptr);

      sFilename = sInFilename;

      pChunk = Mix_LoadWAV(breathe::string::ToUTF8(sFilename).c_str());

      if (pChunk == nullptr) SCREEN<<"cBuffer::Create Mix_LoadWAV FAILED Could not find file \""<<sFilename<<"\""<<std::endl;
      else SCREEN<<"cBuffer::Create loaded file \""<<sFilename<<"\""<<std::endl;
    }


    // ********************************************** cSound **********************************************

    cSource::cSource(audio::cBufferRef pInBuffer) :
      iChannel(-1),

      bLooping(false),
      volume(1.0f),
      pitch(1.0f)
    {
      Create(pInBuffer);
    }

    cSource::~cSource()
    {
      iChannel = 0;
    }

    void cSource::Create(audio::cBufferRef pInBuffer)
    {
      LOG("");

      ASSERT(pBuffer == nullptr);

      ASSERT(pInBuffer != nullptr);
      ASSERT(pInBuffer->IsValid());

      pBuffer = boost::static_pointer_cast<sdlmixer::cBuffer>(pInBuffer);
    }

    void cSource::_SetVolume(float fVolume)
    {
      volume = fVolume;
      if (IsPlaying()) {
        ASSERT(volume >= 0.0f);
        ASSERT(volume <= 1.0f);
        Mix_Volume(iChannel, volume * MIX_MAX_VOLUME);
      }
    }

    void cSource::_SetPitch(float fPitch)
    {
      // Clamp in the range of 0.01f to 2.0f because apparently that is all AL_PITCH is good for
      ASSERT(fPitch > 0.0f);
      ASSERT(fPitch <= 2.0f);
      pitch = math::clamp(fPitch, 0.01f, 2.0f);

      // TODO: This is wrong
      //if (IsPlaying()) alSourcef(iChannel, AL_PITCH, pitch);
    }

    void cSource::_SetLooping()
    {
      bLooping = true;
      /*if (IsPlaying()) {
        source.SetLoop(bLooping);
      }*/
    }

    void cSource::_SetNonLooping()
    {
      bLooping = false;
      ASSERT(!IsPlaying());
    }

    void cSource::_Remove()
    {
      LOG("");

      if (iChannel != 0) {
        LOG("Calling alDeleteSources with iChannel=", iChannel);
        //FIXME: This needs to be called
        //alDeleteSources(1, &iChannel);
        ReportError();
        iChannel = 0;
      }

      // Now remove it from the list
      //cSourceRef temp(this);
      //RemoveSource(temp);
      LOG("returning");
    }

    void cSource::_Play()
    {
      LOG("");

      if (iChannel < 0) {
        if (bLooping) iChannel = Mix_PlayChannel(-1, pBuffer->pChunk, -1);
        else iChannel = Mix_PlayChannel(-1, pBuffer->pChunk, 0);

        if (iChannel < 0) LOG("Mix_PlayChannel FAILED iChannel=", iChannel);

        ReportError();
      }

      SetVolume(volume);
      SetPitch(pitch);
    }

    void cSource::_Stop()
    {
      LOG("");

      if (iChannel >= 0) {
        Mix_HaltChannel(iChannel);
        iChannel = -1;
      }
    }

    void cSource::_Update(durationms_t currentTime, const audio::cListener& listener)
    {
    }

    void cSource::_SetPosition(const spitfire::math::cVec3& _position)
    {
      //LOG("");
      position = _position;
      //alSourcefv(iChannel, AL_POSITION, position.GetPointerConst());
      //ReportError();
    }

    bool cSource::_IsValid() const
    {
      return (pBuffer != nullptr);
    }

    bool cSource::_IsPlaying() const
    {
      if (!IsValid()) return false;

      //LOG("");
      int value = Mix_Playing(iChannel);
      //ReportError();
      return (value == 1);
    }

    void cSource::_SetIsAttachedToScreen()
    {
      LOG("");
      /*alSource3f(iChannel, AL_POSITION, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSource3f(iChannel, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSource3f(iChannel, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
      ReportError();
      alSourcef(iChannel, AL_ROLLOFF_FACTOR, 0.0f);
      ReportError();
      alSourcei(iChannel, AL_SOURCE_RELATIVE, AL_TRUE);
      ReportError();*/
    }


    float_t cSource::GetAttenuation0To1() const
    {
        /*// http://en.wikipedia.org/wiki/Log-distance_path_loss_model
        // fLossDB = (10 * n * log10(d)) + C;
        const float_t n = ...; // path loss exponent
        const float_t C = ...; // constant which accounts for system losses
        const float_t d = fDistanceFromListenerInMeters;

        const float_t fAttenuationFactor = (10.0f * n * log10(d)) + C;*/

        // http://www.gamedev.net/community/forums/topic.asp?topic_id=456691
        // The attenuation formula is f = 1 / (C + Ld + Qd2), where C, L and Q are the constant, linear and quadratic attenuation factors and d is the distance between the vertex being lit and the light source
        const float_t C = 1.0f;
        const float_t L = 0.2f;
        const float_t Q = 0.08f;
        const float_t d = fDistanceFromListenerMeters;

        const float_t fAttenuationFactor0To1 = 1.0f / (C + (L * d) + (Q * math::square(d)));

        ASSERT(fAttenuationFactor0To1 >= 0.0f);
        ASSERT(fAttenuationFactor0To1 <= 1.0f);

        return fAttenuationFactor0To1;
    }

    float_t cSource::GetActualOutputVolume0To1() const
    {
        return fVolume0To1 * (1.0f - GetAttenuation0To1());
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
