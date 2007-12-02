#ifndef AUDIO_H
#define AUDIO_H

// Namespace functions: Global audio system create/destroy, 

// Listener: Position of the player
// 

namespace breathe
{
	namespace audio
	{
		#define AUDIO_MAX_BUFFERS 16

		// Forward declaration
		class cSource;
		class cBuffer;


		bool Init();
		void Destroy();
		void Update(sampletime_t currentTime);

		void Sleep();

		void SetListener(math::cVec3& position, math::cVec3& lookat, math::cVec3& up, math::cVec3& velocity);

		void Add(cSource* pSource);
		void Remove(cSource* pSource);
		
		cBuffer* CreateBuffer(const string_t& sFilename);
		void DestroyBuffer(cBuffer* pBuffer);
		
		cSource* CreateSource(cBuffer* pBuffer, cObject* pObject);
		cSource* CreateSourceAttachedToScreen(cBuffer* pBuffer);
		void DestroySource(cSource* pSource);
		
		void StartAll();
		void StopAll();

		// Buffer to hold the audio data
		class cBuffer
		{
		public:
			explicit cBuffer(const string_t& sFilename);
			~cBuffer();

			bool IsValid() const { return uiBuffer != 0; }

			void Aquire() { ref++; }
			void Release() { ref--; }

			unsigned int uiBuffer;

		private:
			NO_COPY(cBuffer);

			void Create(const string_t& sFilename);

			int ref;
			string_t sFilename;
		};

		// The sound object (Has pointer to node that it is attached to and a pointer to a buffer that it uses)
		class cSource
		{
		public:
			explicit cSource(cBuffer* pBuffer, float fVolume=1.0f);
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

			void SetLooping();
			void SetNonLooping();

		private:
			NO_COPY(cSource);

			bool bLooping;
			unsigned int uiSource;
			float volume;
			
			cBuffer* pBuffer;
			cObject* pNodeParent;
			
			void Create(cBuffer* pBuffer);
		};

		// Very simple wrapper for mixing two sounds together.  
		// You'll basically only do this on a collision between say wood and metal to make the collision
		// sound more natural as opposed to sounding like only wood or only steel
		// Volumes are 0.0f-1.0f, this number is multiplied by the normal level to get the actual volume,
		// so numbers greater than 1.0f will give gain, less than 1.0f will soften the noise sound compared to what
		// it sounds like normally
		class cSourceMix
		{
		public:
			explicit cSourceMix(cBuffer* pBuffer0, cBuffer* pBuffer1, float fVolume0, float fVolume1);

			void Attach(cObject* pNodeParent);
			void Remove();
			void Update();

			void Play();
			void Stop();

			bool IsValid() const;
			bool IsPlaying() const;

		private:
			NO_COPY(cSourceMix);

			cSource source0;
			cSource source1;
		};

		//	Another possible class is 
		//	class cSourceMixN
		//	{
		//	public:
		//		AddSource(cBuffer* pBuffer, float fVolume);
		//	
		//	private:
		//		std::list<cSource> lSource;
		//	};
	}
}

#endif // AUDIO_H
