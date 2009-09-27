#ifndef AUDIO_OGG_H
#define AUDIO_OGG_H

#include <string>
#include <iostream>

#include <AL/al.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

//const size_t BUFFER_SIZE = 32767;
const size_t BUFFER_SIZE = (4096 * 4);
const size_t BUFFER_NUMBER = 4;
//const size_t BUFFER_NUMBER = 2;


namespace breathe
{
  namespace openal
  {
    class cOggStream
    {
    public:
      cOggStream();
      ~cOggStream() { Release(); }

      void Open(const string_t& sPath);

      bool IsPlaying() const;

      void Release();
      void display();
      bool playback();
      bool update();

    private:
      bool RefillBuffer(ALuint buffer);
      void EmptyBuffer();

      FILE* oggFile;
      OggVorbis_File oggStream;
      vorbis_info* vorbisInfo;
      vorbis_comment* vorbisComment;

      ALuint buffers[BUFFER_NUMBER];
      ALuint source;
      ALenum format;
    };
  }
}

#endif // AUDIO_OGG_H
