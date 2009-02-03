#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <vector>
#include <list>

// writing on a text file
#include <iostream>
#include <sstream>
#include <fstream>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <ogg/ogg.h>

#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/thread.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/geometry.h>

#include <breathe/util/base.h>

#include <breathe/audio/audio.h>
#include <breathe/audio/ogg.h>

namespace breathe
{
  namespace audio
  {
    cOggStream::cOggStream() :
      source(0),
      format(0)
    {
      for (size_t i = 0; i < BUFFER_NUMBER; i++) buffers[i] = 0;
    }

    void cOggStream::Open(const string_t& path)
    {
      LOG<<"cOggStream::Open "<<path<<std::endl;

      if(!(oggFile = fopen(breathe::string::ToUTF8(path).c_str(), "rb"))) LOG<<"cOggStream::Open Could not open Ogg file "<<path<<std::endl;

      int result = ov_open(oggFile, &oggStream, NULL, 0);
      if (result < 0) {
        fclose(oggFile);

        LOG<<"cOggStream::Open Could not open Ogg stream "<<path<<" result="<<result<<std::endl;
      }

      vorbisInfo = ov_info(&oggStream, -1);
      vorbisComment = ov_comment(&oggStream, -1);

      if (vorbisInfo->channels == 1) format = AL_FORMAT_MONO16;
      else format = AL_FORMAT_STEREO16;

      alGenBuffers(BUFFER_NUMBER, buffers);
      ReportError();
      alGenSources(1, &source);
      ReportError();

      alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
      alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
      alSource3f(source, AL_DIRECTION, 0.0, 0.0, 0.0);
      alSourcef(source, AL_ROLLOFF_FACTOR, 0.0);
      alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

      LOG<<"cOggStream::Open "<<path<<" successfully opened, returning"<<std::endl;
    }

    void cOggStream::Release()
    {
      alSourceStop(source);
      EmptyBuffer();
      alDeleteSources(1, &source);
      ReportError();
      alDeleteBuffers(BUFFER_NUMBER, buffers);
      ReportError();

      ov_clear(&oggStream);
    }

    void cOggStream::display()
    {
      std::cout
          << "version         " << vorbisInfo->version         << std::endl
          << "channels        " << vorbisInfo->channels        << std::endl
          << "rate (hz)       " << vorbisInfo->rate            << std::endl
          << "bitrate upper   " << vorbisInfo->bitrate_upper   << std::endl
          << "bitrate nominal " << vorbisInfo->bitrate_nominal << std::endl
          << "bitrate lower   " << vorbisInfo->bitrate_lower   << std::endl
          << "bitrate window  " << vorbisInfo->bitrate_window  << std::endl
          << std::endl
          << "vendor " << vorbisComment->vendor << std::endl;

      const size_t n = vorbisComment->comments;
      for (size_t i = 0; i < n; i++)
        std::cout << "   " << vorbisComment->user_comments[i] << std::endl;

      std::cout << std::endl;
    }

    bool cOggStream::playback()
    {
      printf("cOggStream::playback\n");

      if (IsPlaying()) {
        printf("cOggStream::playback already playing, returning true\n");
        return true;
      }

      for (size_t i = 0; i < BUFFER_NUMBER; i++) {
        if (!RefillBuffer(buffers[i])) {
          LOG<<"cOggStream::playback RefillBuffer buffer["<<i<<"] FAILED, returning false"<<std::endl;
          return false;
        }
      }

      printf("cOggStream::playback queueing buffers\n");
      alSourceQueueBuffers(source, BUFFER_NUMBER, buffers);

      printf("cOggStream::playback playing source\n");
      alSourcePlay(source);

      printf("cOggStream::playback successfully started playback, returning true\n");
      return true;
    }

    bool cOggStream::IsPlaying() const
    {
      ALenum state;
      alGetSourcei(source, AL_SOURCE_STATE, &state);
      return (state == AL_PLAYING);
    }

    bool cOggStream::update()
    {
      int processed;
      alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

      bool active = true;
      while (processed--) {
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        ReportError();

        active = RefillBuffer(buffer);

        alSourceQueueBuffers(source, 1, &buffer);
        ReportError();
      }

      return active;
    }

    bool cOggStream::RefillBuffer(ALuint buffer)
    {
      char pcm[BUFFER_SIZE];
      int section;

      int result = 0;
      size_t size = 0;
      while (size < BUFFER_SIZE) {
        result = ov_read(&oggStream, pcm + size, BUFFER_SIZE + size, 0, 2, 1, &section);

        if (result > 0) size += result;
        else {
          LOG<<"cOggStream::RefillBuffer ov_read returned "<<result<<std::endl;
          ReportError();
          break;
        }
      }

      if (size == 0) return false;
      if (result < 0) return false;

      alBufferData(buffer, format, pcm, size, vorbisInfo->rate);
      ReportError();

      return true;
    }

    /*bool cOggStream::RefillBuffer(ALuint buffer)
    {
      char data[BUFFER_SIZE];
      long size = 0;
      int section;

      long read = 0;
      while (size < BUFFER_SIZE) {

        read = ov_read(&oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);

        if (read < 0) return false;

        if (read == 0) ov_time_seek(&oggStream, 0.0);

        size += read;

        if ((BUFFER_SIZE - size) < 4096) break;

        // Yield so that the rest of the program gets chance to process
        breathe::util::YieldThisThread();
      }

      //printf("cOggStream::RefillBuffer alBufferData\n");
      alBufferData(buffer, format, data, size, vorbisInfo->rate);

      //printf("cOggStream::RefillBuffer ReportError\n");
      //ReportError();
      if (alGetError() != AL_NO_ERROR) return false;

      printf("cOggStream::RefillBuffer returning true\n");
      return true;
    }*/

    void cOggStream::EmptyBuffer()
    {
      int queued;
      alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

      while (queued--) {
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        ReportError();
      }
    }
  }
}
