#ifndef SPITFIRE_METADATA_H
#define SPITFIRE_METADATA_H

#include <spitfire/util/cString.h>

namespace spitfire
{
  namespace audio
  {
    // ** cMetaData

    class cMetaData
    {
    public:
      cMetaData();

      void Clear();

      spitfire::string_t sArtist;
      spitfire::string_t sTitle;
      spitfire::string_t sAlbum;
      spitfire::string_t sAlbumArtist;
      spitfire::string_t sGenre;
      spitfire::string_t sComment;
      uint64_t uiYear;
      uint64_t uiTracknum;
      uint64_t uiDurationMilliSeconds;
    };
  }
}

#endif // SPITFIRE_METADATA_H
