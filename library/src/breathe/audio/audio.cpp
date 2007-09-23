// Standard libraries
#include <cmath>

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
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>

#include <BREATHE/GAME/cLevel.h>

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/AUDIO/cAudio.h>

namespace BREATHE
{
	namespace AUDIO
	{
		ALCcontext *Context = NULL;
		ALCdevice *Device = NULL;
		ALboolean g_bEAX = false;
		int iError = 0;

		std::list<cAudioSource*> lAudioSource;
		typedef std::list<cAudioSource*>::iterator iterator;
		
		void AddSource(cAudioSource* pSource)
		{
			lAudioSource.push_back(pSource);
		}

		void RemoveSource(cAudioSource* pSource)
		{
			lAudioSource.remove(pSource);
		}

		bool GetError()
		{
			iError = alGetError();
			return (iError != AL_NO_ERROR);
		}

		void ReportError()
		{
			printf("AL error: %d\n", iError);
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
			iterator iter = lAudioSource.begin();
			iterator iterEnd = lAudioSource.end();

			cAudioSource* pSource = NULL;
			while(iter != iterEnd)
			{
				pSource = (*(iter++));
				if(pSource->IsValid())
					pSource->Play();
			};
		}

		void StopAll()
		{
			iterator iter = lAudioSource.begin();
			iterator iterEnd = lAudioSource.end();

			cAudioSource* pSource = NULL;
			while(iter != iterEnd)
			{
				pSource = (*(iter++));
				if(pSource->IsValid())
					pSource->Play();
			};
		}

		bool Init()
		{
			if(!alutInitWithoutContext(NULL, NULL))
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
			printf("AUDIO::Init %s\n", t.str().c_str());


/*#ifdef PLATFORM_WINDOWS
			char *initString = "DirectSound3D";
#else
			char *initString = "'((direction \"write\")) '((devices '(alsa sdl native null)))";
#endif
			Device = alcOpenDevice((unsigned char *)initString);*/

			Device = alcOpenDevice(NULL); // select the "default device"
			if (Device == NULL)				
				return BREATHE::BAD;

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
			if(eBufferFormat)
				LOG.Success("Audio", "8.1 Surround sound supported");
			else
			{
				eBufferFormat = alGetEnumValue("AL_FORMAT_71CHN16");
				if(eBufferFormat)
					LOG.Success("Audio", "7.1 Surround sound supported");
				else
				{
					eBufferFormat = alGetEnumValue("AL_FORMAT_61CHN16");
					if(eBufferFormat)
						LOG.Success("Audio", "6.1 Surround sound supported");
					else
					{
						eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
						if(eBufferFormat)
							LOG.Success("Audio", "5.1 Surround sound supported");
						else
						{
							eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
							if(eBufferFormat)
								LOG.Success("Audio", "Quad Speaker Surround sound supported");
							else
							{
								eBufferFormat = alGetEnumValue("AL_FORMAT_STEREO16");
								if(eBufferFormat)
									LOG.Success("Audio", "Stereo sound supported");
								else
								{
									eBufferFormat = alGetEnumValue("AL_FORMAT_MONO16");
									if(eBufferFormat)
										LOG.Success("Audio", "Mono sound supported");
									else
										LOG.Success("Audio", "Unknown sound setup");
								}
							}
						}
					}
				}
			}

			return BREATHE::GOOD;
		}

		void Destroy()
		{
			// Exit
			Context=alcGetCurrentContext();
			Device=alcGetContextsDevice(Context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(Context);
			alcCloseDevice(Device);

			if (!alutExit())
				ReportError();
		}

		void Update(float fCurrentTime)
		{
			SetListener(pRender->pFrustum->eye, pRender->pFrustum->target, pRender->pFrustum->up,
				BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));

			iterator iter = lAudioSource.begin();
			iterator iterEnd = lAudioSource.end();

			cAudioSource* pSource = NULL;
			while(iter != iterEnd)
			{
				pSource = (*(iter++));
				if (pSource->IsPlaying())
				{
					pSource->Update();
					continue;
				}
				
				lAudioSource.remove(pSource);
			};
		}

		void SetListener(MATH::cVec3& position, MATH::cVec3& lookat, MATH::cVec3& up, MATH::cVec3& velocity)
		{
			ALfloat listenerOri[]={lookat.x, lookat.y, lookat.z, up.x, up.y, up.z};

			alListenerfv(AL_POSITION, position);
			alListenerfv(AL_VELOCITY, velocity);
			alListenerfv(AL_ORIENTATION, listenerOri);
		}
		
		// ********************************************** cAudioBuffer **********************************************

		cAudioBuffer::cAudioBuffer(std::string sInFilename, bool bInLooping)
			: uiBuffer(0), bLooping(bInLooping), sFilename()
		{
			Create(sInFilename);
		}

		cAudioBuffer::~cAudioBuffer()
		{
			if(uiBuffer) alDeleteBuffers(1, &uiBuffer);
		}

    void cAudioBuffer::Create(std::string sInFilename)
		{
			sFilename = sInFilename;
			uiBuffer = alutCreateBufferFromFile(sFilename.c_str());
			ReportError();
		}

		// ********************************************** cAudioSound **********************************************

		cAudioSource::cAudioSource(cAudioBuffer* pInBuffer, float fVolume)
			: uiSource(0), pBuffer(NULL), pNodeParent(NULL), volume(fVolume)
		{
			Create(pInBuffer);
		}

		cAudioSource::~cAudioSource()
		{
			uiSource = 0;
			pBuffer = NULL;
			pNodeParent = NULL;
		}

    void cAudioSource::Create(cAudioBuffer* pInBuffer)
		{
			pBuffer = pInBuffer;
			alGenSources(1, &uiSource);
			ReportError();
			alSourcei (uiSource, AL_BUFFER, pInBuffer->uiBuffer);
			ReportError();

			// If this buffer is a looping buffer then we have to be a looping source
			if(pBuffer->IsLooping())
				alSourcei(uiSource, AL_LOOPING, AL_TRUE);
		}

		void cAudioSource::Attach(cObject* pInNodeParent)
		{
			pNodeParent = pInNodeParent;
			AddSource(this);
		}
		
		void cAudioSource::Remove()
		{
			pNodeParent = NULL;
			RemoveSource(this);
			alDeleteSources(1, &uiSource);
			uiSource = 0;
		}

		void cAudioSource::Play()
		{
			printf("Playing source\n");
			alSourcePlay (uiSource);
      ReportError();
		}

		void cAudioSource::Stop()
		{
			alSourceStop(uiSource);
		}

		void cAudioSource::Update()
		{
			alSourcefv(uiSource, AL_POSITION, pNodeParent->p);
		}

		bool cAudioSource::IsValid()
		{
			return uiSource && pBuffer && pNodeParent;
		}

		bool cAudioSource::IsPlaying()
		{
			if(!IsValid()) return false;

			// If we have stopped playing this sound remove us from the list
			ALint value = AL_PLAYING;
			alGetSourcei(uiSource, AL_SOURCE_STATE, &value);
			return (AL_PLAYING == value);
		}

		void cAudioSource::TransformTo2DSource()
		{
			alSourcei (uiSource, AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef (uiSource, AL_ROLLOFF_FACTOR, 0.0);
		}

		void cAudioSource::TransformTo3DSource()
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
		cAudioSourceMix::cAudioSourceMix(cAudioBuffer* pBuffer0, cAudioBuffer* pBuffer1, float fVolume0, float fVolume1) :
			source0(pBuffer0, fVolume0),
			source1(pBuffer1, fVolume1)
		{

		}

		void cAudioSourceMix::Attach(cObject* pNodeParent)
		{
			source0.Attach(pNodeParent);
			source1.Attach(pNodeParent);
		}
		
		void cAudioSourceMix::Remove()
		{
			source0.Remove();
			source1.Remove();
		}
		
		void cAudioSourceMix::Update()
		{
			source0.Update();
			source1.Update();
		}
		
		void cAudioSourceMix::Play()
		{
			source0.Play();
			source1.Play();
		}
		
		void cAudioSourceMix::Stop()
		{
			source0.Stop();
			source1.Stop();
		}
		
		bool cAudioSourceMix::IsValid()
		{
			return source0.IsValid() && source1.IsValid();
		}
		
		bool cAudioSourceMix::IsPlaying()
		{
			return source0.IsPlaying() && source1.IsPlaying();
		}
	}
}
