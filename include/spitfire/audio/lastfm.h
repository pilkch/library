#ifndef SPITFIRE_LASTFM_H
#define SPITFIRE_LASTFM_H

// This is a simple wrapper around libclastfm
// http://liblastfm.sourceforge.net/

// Standard headers
#include <string>

// libclastfm headers
extern "C"{
#include <clastfm.h>
}

// Spitfire headers
#include <spitfire/audio/metadata.h>

// Define these in your cmake file
//#define BUILD_LASTFM_KEY "<mykey>"
//#define BUILD_LASTFM_SECRET "<mysecret>"

namespace spitfire
{
  namespace audio
  {
    // ** cLastFM
    // http://www.last.fm/api/scrobbling
    //
    // Submission Rules
    // The track must be longer than 30 seconds.
    // And the track has been played for at least half its duration, or for 4 minutes (whichever occurs earlier.)
    //
    class cLastFM
    {
    public:
      cLastFM();
      ~cLastFM();

      bool IsLoggedIn() const;
      void Login(const string_t& sUserName, const string_t& sPassword);

      void Scrobble(const cMetaData& metaData);
      void UpdateTrack(const cMetaData& metaData);

      void LoveTrack(const cMetaData& metaData);

    private:
      void LogOut();

      void ScrobbleOrUpdateTrack(const cMetaData& metaData, bool bScrobble);

      LASTFM_SESSION* pSession;
    };
  }
}

#endif // SPITFIRE_LASTFM_H
