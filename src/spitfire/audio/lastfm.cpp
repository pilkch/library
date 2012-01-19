// Standard headers
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <cstdio>

// Spitfire headers
#include <spitfire/audio/lastfm.h>

namespace spitfire
{
  namespace audio
  {
    cLastFM::cLastFM() :
      pSession(nullptr)
    {
    }

    cLastFM::~cLastFM()
    {
      LogOut();
    }

    bool cLastFM::IsLoggedIn() const
    {
      return (pSession != nullptr);
    }

    void cLastFM::Login(const string_t& sUserName, const string_t& sPassword)
    {
      LogOut();

      const char* szKey = BUILD_LASTFM_KEY;
      const char* szSecret = BUILD_LASTFM_SECRET;

      pSession = LASTFM_init(szKey, szSecret);

      const std::string sUserNameUTF8 = spitfire::string::ToUTF8(sUserName);
      const std::string sPasswordUTF8 = spitfire::string::ToUTF8(sPassword);

      int iResult = LASTFM_login(pSession, sUserNameUTF8.c_str(), sPasswordUTF8.c_str());
      if (iResult != 0) {
        std::cerr<<"cLastFM::Scrobble LASTFM_login FAILED status="<<LASTFM_status(pSession)<<"\n";
        LogOut();
      }
    }

    void cLastFM::LogOut()
    {
      if (pSession != nullptr) {
        LASTFM_dinit(pSession);
        pSession = nullptr;
      }
    }

    void cLastFM::ScrobbleOrUpdateTrack(const cMetaData& metaData, bool bScrobble)
    {
      if (!IsLoggedIn()) {
        std::cerr<<"cLastFM::ScrobbleOrUpdateTrack Error not logged in to LastFM\n";
        return;
      }

      const std::string sArtistUTF8 = spitfire::string::ToUTF8(metaData.sArtist);
      const std::string sTitleUTF8 = spitfire::string::ToUTF8(metaData.sTitle);
      const std::string sAlbumUTF8 = spitfire::string::ToUTF8(metaData.sAlbum);

      char* szArtistUTF8 = new char[sArtistUTF8.length() + 1];
      strcpy(szArtistUTF8, sArtistUTF8.c_str());
      char* szTitleUTF8 = new char[sTitleUTF8.length() + 1];
      strcpy(szTitleUTF8, sTitleUTF8.c_str());
      char* szAlbumUTF8 = new char[sAlbumUTF8.length() + 1];
      strcpy(szAlbumUTF8, sAlbumUTF8.c_str());

      const uint64_t uiLengthSeconds = metaData.uiDurationMilliSeconds / 1000;

      if (bScrobble) {
        /*time_t rawtime;
        time(&rawtime);

        tm* ptm = gmtime(&rawtime);

        time_t started = mktime(ptm);
        if (ptm->tm_isdst) {
          ptm->tm_hour += 1;
          started = mktime(ptm);
        }*/

        // TODO: Start a background thread so we don't have to do this
        time_t started;
        time(&started);
        started -= 31; // simulate playtime

        LASTFM_track_scrobble(pSession, szTitleUTF8, szAlbumUTF8, szArtistUTF8, started, uiLengthSeconds, 0, 0, NULL);
      } else LASTFM_track_update_now_playing(pSession, szTitleUTF8, szAlbumUTF8, szArtistUTF8, uiLengthSeconds, 0, 0);

      delete [] szAlbumUTF8;
      delete [] szTitleUTF8;
      delete [] szArtistUTF8;

      std::cout<<"cLastFM::ScrobbleOrUpdateTrack returning, status="<<LASTFM_status(pSession)<<"\n";
    }

    void cLastFM::Scrobble(const cMetaData& metaData)
    {
      const bool bScrobble = true;
      ScrobbleOrUpdateTrack(metaData, bScrobble);
    }

    void cLastFM::UpdateTrack(const cMetaData& metaData)
    {
      const bool bScrobble = false;
      ScrobbleOrUpdateTrack(metaData, bScrobble);
    }

    void cLastFM::LoveTrack(const cMetaData& metaData)
    {
      if (!IsLoggedIn()) {
        std::cerr<<"cLastFM::LoveTrack Error not logged in to LastFM\n";
        return;
      }

      const std::string sArtistUTF8 = spitfire::string::ToUTF8(metaData.sArtist);
      const std::string sTitleUTF8 = spitfire::string::ToUTF8(metaData.sTitle);

      LASTFM_track_love(pSession, sTitleUTF8.c_str(), sArtistUTF8.c_str());

      std::cout<<"cLastFM::LoveTrack returning, status="<<LASTFM_status(pSession)<<"\n";
    }
  }
}
