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
			cAudioSource(cAudioBuffer* pBuffer);
			~cAudioSource();

			void Attach(cObject* pNodeParent);
			void Play();
			void Remove();
			void Update();

			bool IsValid() { return uiSource && pBuffer && pNodeParent; }

			unsigned int uiSource;
			cAudioBuffer* pBuffer;

		private:
			void Create(cAudioBuffer* pBuffer);

			cObject* pNodeParent;
		};
	}
}

#endif // CAUDIO_H
