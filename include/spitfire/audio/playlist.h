#ifndef SPITFIRE_PLAYLIST_H
#define SPITFIRE_PLAYLIST_H

// Standard headers
#include <list>

// Spitfire headers
#include <spitfire/util/string.h>
#include <spitfire/storage/file.h>

namespace spitfire
{
  namespace audio
  {
    class cTrack
    {
    public:
      cTrack();

      string_t sFullPath;

      string_t sArtist;
      string_t sTitle;
      uint64_t uiTrackLengthMS;
    };

    // ** cPlaylist

    class cPlaylist
    {
    public:
      ~cPlaylist();

      void Clear();

      std::vector<cTrack*> tracks;
    };


    //** cPlayListReader

    class cPlayListReader {
    public:
      bool IsPlayListFile(const string_t& sFilePath) const;

      bool Load(const string_t& sFilePath, cPlaylist& playlist) const;

    private:
      bool LoadFromM3u(const string_t& sFilePath, cPlaylist& playlist) const;
      bool LoadFromPls(const string_t& sFilePath, cPlaylist& playlist) const;
    };


    //** cPlayListWriter

    class cPlayListWriter {
    public:
      bool Save(const string_t& sFilePath, const cPlaylist& playlist) const;

    private:
      bool SaveToM3u(const string_t& sFilePath, const cPlaylist& playlist) const;
      bool SaveToPls(const string_t& sFilePath, const cPlaylist& playlist) const;

      string_t GetTrackTitle(const cTrack& track) const;
    };
  }
}

#endif // SPITFIRE_PLAYLIST_H
