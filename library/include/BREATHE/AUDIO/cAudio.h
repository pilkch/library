#ifndef CAUDIO_H
#define CAUDIO_H

namespace BREATHE
{
	namespace AUDIO
	{
		#define AUDIO_MAX_BUFFERS 32

		// Forward declaration
		class cAudioSource;


		bool Init();
		void Destroy();
		void Update(float fCurrentTime);

		void Sleep();

		void SetListener(MATH::cVec3& position, MATH::cVec3& lookat, MATH::cVec3& up, MATH::cVec3& velocity);

		void Add(cAudioSource* pSource);
		void Remove(cAudioSource* pSource);

		// Buffer to hold the audio data
		class cAudioBuffer
		{
		public:
			cAudioBuffer(std::string sFilename, bool bLooping=false);
			~cAudioBuffer();

			bool IsLooping() { return bLooping; }
			bool IsValid() { return uiBuffer != 0; }

			unsigned int uiBuffer;

		private:
			void Create(std::string sFilename);

			bool bLooping;
			std::string sFilename;
		};

		// The sound object (Has pointer to node that it is attached to and a pointer to a buffer that it uses)
		class cAudioSource
		{
		public:
			cAudioSource(cAudioBuffer* pBuffer, float fVolume=1.0f);
			~cAudioSource();

			void Attach(cObject* pNodeParent);
			void Remove();
			void Update();

			void Play();
			void Stop();

			bool IsValid();
			bool IsPlaying();

		private:
			unsigned int uiSource;
			float volume;
			
			cAudioBuffer* pBuffer;
			cObject* pNodeParent;
			
			void Create(cAudioBuffer* pBuffer);
		};

		// Very simple wrapper for mixing two sounds together.  
		// You'll basically only do this on a collision between say wood and metal to make the collision
		// sound more natural as opposed to sounding like only wood or only steel
		// Volumes are 0.0f-1.0f, this number is multiplied by the normal level to get the actual volume,
		// so numbers greater than 1.0f will give gain, less than 1.0f will soften the noise sound compared to what
		// it sounds like normally
		class cAudioSourceMix
		{
		public:
			cAudioSourceMix(cAudioBuffer* pBuffer0, cAudioBuffer* pBuffer1, float fVolume0, float fVolume1);

			void Attach(cObject* pNodeParent);
			void Remove();
			void Update();

			void Play();
			void Stop();

			bool IsValid();
			bool IsPlaying();

		private:
			cAudioSource source0;
			cAudioSource source1;
		};

		//	Another possible class is 
		//	class cAudioSourceMixN
		//	{
		//	public:
		//		AddSource(cAudioBuffer* pBuffer, float fVolume);
		//	
		//	private:
		//		std::list<cAudioSource> lSource;
		//	};
	}
}

#endif // CAUDIO_H
