// Standard libraries
#include <cmath>
#include <cassert>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>

// writing on a text file
#include <iostream>
#include <sstream>
#include <fstream>

// Other libraries
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/game/cLevel.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/audio/audio.h>

namespace breathe
{
	namespace audio
	{
		ALCcontext* Context = nullptr;
		ALCdevice* Device = nullptr;
		ALboolean g_bEAX = false;
		int iError = 0;

		std::map<string_t, cBuffer*> mAudioBuffer;
		typedef std::map<string_t, cBuffer*> ::iterator buffer_iterator;

		cBuffer* GetAudioBuffer(const string_t& sFilename)
		{
			cBuffer* pBuffer = mAudioBuffer[sFilename];

			if (pBuffer != nullptr) return pBuffer;

			pBuffer = new cBuffer(sFilename);
			assert(pBuffer != nullptr);

			mAudioBuffer[sFilename] = pBuffer;

			return pBuffer;
		}

		std::list<cSource*> lAudioSource;
		typedef std::list<cSource*>::iterator source_iterator;

		void AddSource(cSource* pSource)
		{
			lAudioSource.push_back(pSource);
		}

		void RemoveSource(cSource* pSource)
		{
			lAudioSource.remove(pSource);
		}


		cBuffer* CreateBuffer(const string_t& sFilename)
		{
			return GetAudioBuffer(sFilename);
		}

		void DestroyBuffer(cBuffer* pBuffer)
		{

		}

		cSource* CreateSourceAttachedToObject(cBuffer* pBuffer, cObject* pObject)
		{
			assert(pBuffer != nullptr);
      assert(pBuffer->IsValid());

			breathe::audio::cSource* pSource = new breathe::audio::cSource(pBuffer);
			assert(pSource != nullptr);
      assert(pSource->IsValid());

      AddSource(pSource);

			pSource->Attach(pObject);

			return pSource;
		}

		cSource* CreateSourceAttachedToScreen(cBuffer* pBuffer)
		{
			assert(pBuffer != nullptr);
      assert(pBuffer->IsValid());

			breathe::audio::cSource* pSource = new breathe::audio::cSource(pBuffer);
			assert(pSource != nullptr);
      assert(pSource->IsValid());

      AddSource(pSource);

      unsigned int source = pSource->GetSource();
      alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
      alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      alSource3f(source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
      alSourcef(source, AL_ROLLOFF_FACTOR, 0.0f);
      alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

			return pSource;
		}

		void DestroySource(cSource* pSource)
		{

		}

		bool GetError()
		{
			iError = alGetError();
			return (iError != AL_NO_ERROR);
		}

		void ReportError()
		{
			if (iError != AL_NO_ERROR) SCREEN<<"AL error: "<<iError<<std::endl;
		}

		void GetAndReportError()
		{
			GetError();
			ReportError();
		}

		void Sleep()
		{
			alutSleep (3);
		}

		void StartAll()
		{
			source_iterator iter = lAudioSource.begin();
			source_iterator iterEnd = lAudioSource.end();

			cSource* pSource = NULL;
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

			cSource* pSource = NULL;
			while(iter != iterEnd)
			{
				pSource = (*(iter++));
				if (pSource->IsValid())
					pSource->Play();
			};
		}

		bool Init()
		{
			if (!alutInitWithoutContext(NULL, NULL))
				ReportError();

			// Major minor version number
			ALint iMajor = alutGetMajorVersion();
			ALint iMinor = alutGetMinorVersion();
			std::ostringstream t;
			t<<"OpenAL v";
			t<<iMajor;
			t<<".";
			t<<iMinor;
			LOG.Success("Audio", t.str());
			printf("audio::Init %s\n", t.str().c_str());


/*#ifdef __WIN__
			char* initString = "DirectSound3D";
#else
			char* initString = "'((direction \"write\")) '((devices '(alsa sdl native null)))";
#endif
			Device = alcOpenDevice((unsigned char*)initString);*/

			Device = alcOpenDevice(NULL); // select the "default device"
			if (Device == NULL)
				return breathe::BAD;

			// Create our context
			Context=alcCreateContext(Device,NULL);
			alcMakeContextCurrent(Context);

			// Check for EAX 2.0 support
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
			ALenum eBufferFormat = 0;
			eBufferFormat = alGetEnumValue("AL_FORMAT_81CHN16");
			if (eBufferFormat)
				LOG.Success("Audio", "8.1 Surround sound supported");
			else
			{
				eBufferFormat = alGetEnumValue("AL_FORMAT_71CHN16");
				if (eBufferFormat)
					LOG.Success("Audio", "7.1 Surround sound supported");
				else
				{
					eBufferFormat = alGetEnumValue("AL_FORMAT_61CHN16");
					if (eBufferFormat)
						LOG.Success("Audio", "6.1 Surround sound supported");
					else
					{
						eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
						if (eBufferFormat)
							LOG.Success("Audio", "5.1 Surround sound supported");
						else
						{
							eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
							if (eBufferFormat)
								LOG.Success("Audio", "Quad Speaker Surround sound supported");
							else
							{
								eBufferFormat = alGetEnumValue("AL_FORMAT_STEREO16");
								if (eBufferFormat)
									LOG.Success("Audio", "Stereo sound supported");
								else
								{
									eBufferFormat = alGetEnumValue("AL_FORMAT_MONO16");
									if (eBufferFormat)
										LOG.Success("Audio", "Mono sound supported");
									else
										LOG.Success("Audio", "Unknown sound setup");
								}
							}
						}
					}
				}
			}

			return breathe::GOOD;
		}

		void Destroy()
		{
			// Exit
			Context=alcGetCurrentContext();
			Device=alcGetContextsDevice(Context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(Context);

      if (Device != nullptr) alcCloseDevice(Device);

      if (!alutExit()) ReportError();
    }

		void Update(sampletime_t currentTime)
		{
      math::cVec3 position;
      SetListener(pRender->pFrustum->eye, pRender->pFrustum->target, pRender->pFrustum->up, position);

			source_iterator iter = lAudioSource.begin();
			source_iterator iterEnd = lAudioSource.end();

      std::list<cSource*> listToRemove;

			cSource* pSource = NULL;
			while(iter != iterEnd)
			{
				pSource = (*(iter++));
				if (pSource->IsPlaying())
				{
					pSource->Update();
					continue;
				}

        // Add it to the list of sources to remove, leave the removing until the end
				listToRemove.push_back(pSource);
			};


			iter = listToRemove.begin();
			iterEnd = listToRemove.end();

			while(iter != iterEnd)
			{
				pSource = (*(iter++));
        pSource->Remove();
        SAFE_DELETE(pSource);
      }
		}

		void SetListener(math::cVec3& position, math::cVec3& lookat, math::cVec3& up, math::cVec3& velocity)
		{
			ALfloat listenerOri[]={lookat.x, lookat.y, lookat.z, up.x, up.y, up.z};

			alListenerfv(AL_POSITION, position);
			alListenerfv(AL_VELOCITY, velocity);
			alListenerfv(AL_ORIENTATION, listenerOri);
		}

    void CreateSoundAttachedToScreenPlayAndForget(const breathe::string_t& sFilename)
    {
	    breathe::audio::cBuffer* pBuffer = breathe::audio::CreateBuffer(sFilename);
	    assert(pBuffer != nullptr);

	    breathe::audio::cSource* pSource = breathe::audio::CreateSourceAttachedToScreen(pBuffer);
	    assert(pSource != nullptr);

	    pSource->Play();
    }



		// ********************************************** cBuffer **********************************************

		cBuffer::cBuffer(const string_t& sInFilename) :
			uiBuffer(0),
			ref(0),
			sFilename()
		{
			Create(sInFilename);
		}

		cBuffer::~cBuffer()
		{
			assert(ref == 0);
			if (uiBuffer) alDeleteBuffers(1, &uiBuffer);
		}

    void cBuffer::Create(const string_t& sInFilename)
		{
			sFilename = sInFilename;
			uiBuffer = alutCreateBufferFromFile(breathe::string::ToUTF8(sFilename).c_str());
			ReportError();

			if (uiBuffer == 0) SCREEN<<"Audio could not find file \""<<breathe::string::ToUTF8(sFilename)<<"\""<<std::endl;
			else SCREEN<<"Audio found file \""<<breathe::string::ToUTF8(sFilename)<<"\" uiBuffer="<<uiBuffer<<std::endl;
		}


		// ********************************************** cSound **********************************************

		cSource::cSource(cBuffer* pInBuffer, float fVolume) :
			pBuffer(nullptr),
			bLooping(false),
			pNodeParent(nullptr),
			uiSource(0),
			volume(fVolume)
		{
			Create(pInBuffer);
		}

		cSource::~cSource()
		{
			assert(pBuffer != nullptr);

			pBuffer->Release();
			pBuffer = nullptr;

			uiSource = 0;
			pNodeParent = nullptr;
		}

    void cSource::Create(cBuffer* pInBuffer)
		{
			assert(pBuffer == nullptr);

			pBuffer = pInBuffer;
			pBuffer->Aquire();
			alGenSources(1, &uiSource);
			ReportError();
			alSourcei(uiSource, AL_BUFFER, pInBuffer->uiBuffer);
			ReportError();
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
			AddSource(this);
		}

		void cSource::Remove()
		{
			pNodeParent = NULL;
			RemoveSource(this);
			alDeleteSources(1, &uiSource);
			uiSource = 0;
		}

		void cSource::Play()
		{
			alSourcePlay(uiSource);
      ReportError();
		}

		void cSource::Stop()
		{
			alSourceStop(uiSource);
		}

		void cSource::Update()
		{
      if (pNodeParent != nullptr) alSourcefv(uiSource, AL_POSITION, pNodeParent->p);
		}

		bool cSource::IsValid() const
		{
			return uiSource && pBuffer;
		}

		bool cSource::IsPlaying() const
		{
			if (!IsValid()) return false;

			// If we have stopped playing this sound remove us from the list
			ALint value = AL_PLAYING;
			alGetSourcei(uiSource, AL_SOURCE_STATE, &value);
			return (AL_PLAYING == value);
		}

		void cSource::TransformTo2DSource()
		{
			alSourcei(uiSource, AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef(uiSource, AL_ROLLOFF_FACTOR, 0.0);
		}

		void cSource::TransformTo3DSource()
		{
			alSourcei (uiSource, AL_SOURCE_RELATIVE, AL_FALSE);
			alSourcef (uiSource, AL_ROLLOFF_FACTOR, 1.0);
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
			Context=alcGetCurrentContext();
			Device=alcGetContextsDevice(Context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(Context);
			alcCloseDevice(Device);






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
		cSourceMix::cSourceMix(cBuffer* pBuffer0, cBuffer* pBuffer1, float fVolume0, float fVolume1) :
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
