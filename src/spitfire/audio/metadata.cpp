// Meduasa headers
#include <spitfire/audio/metadata.h>

namespace spitfire
{
  namespace audio
  {
    // ** cMetaData

    cMetaData::cMetaData() :
      uiYear(0),
      uiTracknum(0),
      uiDurationMilliSeconds(0)
    {
    }

    void cMetaData::Clear()
    {
      sArtist.clear();
      sCompilationArtist.clear();
      sAlbum.clear();
      sTitle.clear();
      sGenre.clear();
      sComment.clear();
      uiYear = 0;
      uiTracknum = 0;
      uiDurationMilliSeconds = 0;
    }
  }
}
