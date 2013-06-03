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
    namespace lastfm
    {
      cSession::cSession(const string_t& sKey, const string_t& sSecret) :
        pSession(nullptr),
        bIsLoggedIn(false)
      {
        pSession = LASTFM_init(sKey.c_str(), sSecret.c_str());
      }

      cSession::~cSession()
      {
        if (pSession != nullptr) {
          LASTFM_dinit(pSession);
          pSession = nullptr;
        }
      }

      bool cSession::IsLoggedIn() const
      {
        return bIsLoggedIn;
      }

      void cSession::Login(const string_t& sUserName, const string_t& sPassword)
      {
        bIsLoggedIn = false;

        const std::string sUserNameUTF8 = spitfire::string::ToUTF8(sUserName);
        const std::string sPasswordUTF8 = spitfire::string::ToUTF8(sPassword);

        int iResult = LASTFM_login(pSession, sUserNameUTF8.c_str(), sPasswordUTF8.c_str());
        if (iResult != 0) {
          const char* szStatus = nullptr;
          const int* iErrorCode = nullptr;
          const char* szErrorText = nullptr;
          LASTFM_status(pSession, &szStatus, &iErrorCode, &szErrorText);
          std::cerr<<"cSession::Scrobble LASTFM_login FAILED status="<<szStatus<<", error="<<iErrorCode<<", "<<szErrorText<<std::endl;
        } else bIsLoggedIn = true;
      }

      bool cSession::ScrobbleOrUpdateTrack(const cMetaData& metaData, bool bScrobble)
      {
        std::cout<<"cSession::ScrobbleOrUpdateTrack "<<metaData.sArtist<<" - "<<metaData.sTitle<<std::endl;

        if (!IsLoggedIn()) {
          std::cerr<<"cSession::ScrobbleOrUpdateTrack Error not logged in to LastFM"<<std::endl;
          return false;
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

        int iResult = -1;

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

          iResult = LASTFM_track_scrobble(pSession, szTitleUTF8, szAlbumUTF8, szArtistUTF8, started, uiLengthSeconds, 0, 0, nullptr);
        } else iResult = LASTFM_track_update_now_playing(pSession, szTitleUTF8, szAlbumUTF8, szArtistUTF8, uiLengthSeconds, 0, 0, nullptr);

        delete [] szAlbumUTF8;
        delete [] szTitleUTF8;
        delete [] szArtistUTF8;

        const char* szStatus = nullptr;
        const int* iErrorCode = nullptr;
        const char* szErrorText = nullptr;
        LASTFM_status(pSession, &szStatus, &iErrorCode, &szErrorText);
        std::cout<<"cSession::ScrobbleOrUpdateTrack returning "<<(iResult == 0)<<", status="<<szStatus<<", error="<<iErrorCode<<", "<<szErrorText<<std::endl;
        return (iResult == 0);
      }

      bool cSession::Scrobble(const cMetaData& metaData)
      {
        const bool bScrobble = true;
        return ScrobbleOrUpdateTrack(metaData, bScrobble);
      }

      bool cSession::UpdateTrack(const cMetaData& metaData)
      {
        const bool bScrobble = false;
        return ScrobbleOrUpdateTrack(metaData, bScrobble);
      }

      bool cSession::LoveTrack(const cMetaData& metaData)
      {
        std::cout<<"cSession::LoveTrack "<<metaData.sArtist<<" - "<<metaData.sTitle<<std::endl;

        if (!IsLoggedIn()) {
          std::cerr<<"cSession::LoveTrack Error not logged in to LastFM"<<std::endl;
          return false;
        }

        const std::string sArtistUTF8 = spitfire::string::ToUTF8(metaData.sArtist);
        const std::string sTitleUTF8 = spitfire::string::ToUTF8(metaData.sTitle);

        int iResult = LASTFM_track_love(pSession, sTitleUTF8.c_str(), sArtistUTF8.c_str());

        const char* szStatus = nullptr;
        const int* iErrorCode = nullptr;
        const char* szErrorText = nullptr;
        LASTFM_status(pSession, &szStatus, &iErrorCode, &szErrorText);
        std::cout<<"cSession::LoveTrack returning "<<(iResult == 0)<<", status="<<szStatus<<", error="<<iErrorCode<<", "<<szErrorText<<std::endl;
        return (iResult == 0);
      }


      // ** cLastFM

      cLastFM::~cLastFM()
      {
        // Remove any further events because we don't care any more
        while (true) {
          cEvent* pEvent = eventQueue.RemoveItemFromFront();
          if (pEvent == nullptr) break;

          SAFE_DELETE(pEvent);
        }
      }

      void cLastFM::ReadListenedFromFile(std::list<cEntry*>& listened)
      {
        util::cLockObject lock(mutex);
      }

      void cLastFM::WriteListenedToFile(const std::list<cEntry*>& listened) const
      {
        util::cLockObject lock(mutex);
      }

      void cLastFM::ReadLovedFromFile(std::list<cEntry*>& loved)
      {
        util::cLockObject lock(mutex);
      }

      void cLastFM::WriteLovedToFile(const std::list<cEntry*>& loved) const
      {
        util::cLockObject lock(mutex);
      }

      void cLastFM::StartPlayingTrack(const cMetaData& metaData)
      {
        cEvent* pEvent = new cEvent;
        pEvent->type = cEvent::TYPE::START_TRACK;
        pEvent->metaData = metaData;
        eventQueue.AddItemToBack(pEvent);
      }

      void cLastFM::StopPlayingTrack()
      {
        cEvent* pEvent = new cEvent;
        pEvent->type = cEvent::TYPE::STOP_TRACK;
        eventQueue.AddItemToBack(pEvent);
      }

      void cLastFM::LoveTrack(const cMetaData& metaData)
      {
        cEvent* pEvent = new cEvent;
        pEvent->type = cEvent::TYPE::LOVE_TRACK;
        pEvent->metaData = metaData;
        eventQueue.AddItemToBack(pEvent);
      }

      void cLastFM::ThreadFunction()
      {
        std::cout<<"cLastFM::ThreadFunction"<<std::endl;

        // Check that we can log in
        cSession session(sKey, sSecret);
        session.Login(sUserName, sPassword);

        if (!session.IsLoggedIn()) {
          std::cout<<"cLastFM::ThreadFunction Could not log in to last.fm, returning"<<std::endl;
          return;
        }

        cEntry* pCurrent = nullptr;
        bool bAddedCurrentSongToListenedList = false;
        util::cDateTime lastScrobbled;

        std::list<cEntry*> listened;
        ReadListenedFromFile(listened);

        std::list<cEntry*> loved;
        ReadLovedFromFile(loved);

        while (true) {
          //std::cout<<"cLastFM::ThreadFunction Loop"<<std::endl;
          soAction.WaitTimeoutMS(1000);

          if (IsToStop()) break;

          cEvent* pEvent = eventQueue.RemoveItemFromFront();
          if (pEvent != nullptr) {
            switch (pEvent->type) {
              case cEvent::TYPE::START_TRACK: {
                SAFE_DELETE(pCurrent);
                if (pCurrent == nullptr) {
                  pCurrent = new cEntry;
                  pCurrent->dateTime = pEvent->dateTime;
                  pCurrent->metaData = pEvent->metaData;
                  bAddedCurrentSongToListenedList = false;
                }
                break;
              }
              case cEvent::TYPE::STOP_TRACK: {
                SAFE_DELETE(pCurrent);
                bAddedCurrentSongToListenedList = false;
                break;
              }
              case cEvent::TYPE::LOVE_TRACK: {
                std::cerr<<"cLastFM::ThreadFunction Love has not been implemented"<<std::endl;
                break;
              }
            }

            SAFE_DELETE(pEvent);
          }

          if (IsToStop()) break;

          // Check currently playing some
          if ((pCurrent != nullptr) && !bAddedCurrentSongToListenedList) {
            //std::cout<<"cLastFM::ThreadFunction Checking song"<<std::endl;
            const util::cDateTime now;
            if (now.GetMillisecondsSince0AD() >= pCurrent->dateTime.GetMillisecondsSince0AD()) {
              uint64_t uiPositionMS = now.GetMillisecondsSince0AD() - pCurrent->dateTime.GetMillisecondsSince0AD();
              // If track is more than 30 second long and (position is at least half way or position is at least 4 minutes
              if ((pCurrent->metaData.uiDurationMilliSeconds > (30 * 1000)) && ((uiPositionMS >= (pCurrent->metaData.uiDurationMilliSeconds / 2)) || (uiPositionMS >= (4 * 60 * 1000)))) {
                cEntry* pNewEntry = new cEntry;
                *pNewEntry = *pCurrent;
                listened.push_back(pNewEntry);

                bAddedCurrentSongToListenedList = true;

                // Update listened
                WriteListenedToFile(listened);
              }
            }
          }

          if (IsToStop()) break;

          // Send up to 10 songs from listened to last.fm
          if (!listened.empty()) {
            const util::cDateTime now;
            ASSERT(now.GetMillisecondsSince0AD() >= lastScrobbled.GetMillisecondsSince0AD());
            uint64_t uiTimeSinceLastScrobbleMS = now.GetMillisecondsSince0AD() - lastScrobbled.GetMillisecondsSince0AD();
            if (uiTimeSinceLastScrobbleMS > 10000) {
              if (session.IsLoggedIn()) {
                bool bScrobbledAtLeastOneSong = false;

                for (size_t i = 0; i < 10; i++) {
                  // Break if we are told to stop or listened is now empty
                  if (IsToStop() || listened.empty()) break;

                  cEntry* pEntry = listened.front();
                  ASSERT(pEntry != nullptr);

                  // Only scrobble the song if it has a valid artist and title
                  const bool bIsMetaDataValid = (!pEntry->metaData.sArtist.empty() && !pEntry->metaData.sTitle.empty());

                  if (bIsMetaDataValid && !session.Scrobble(pEntry->metaData)) {
                    std::cerr<<"cLastFM::ThreadFunction Error scrobbling \""<<pEntry->metaData.sArtist<<"\" - \""<<pEntry->metaData.sTitle<<"\""<<std::endl;
                    break;
                  }

                  listened.pop_front();

                  SAFE_DELETE(pEntry);

                  bScrobbledAtLeastOneSong = true;
                }

                if (bScrobbledAtLeastOneSong) {
                  WriteListenedToFile(listened);
                  lastScrobbled = now;
                }
              }
            }
          }
        }

        SAFE_DELETE(pCurrent);

        {
          std::list<cEntry*>::iterator iter = listened.begin();
          const std::list<cEntry*>::iterator iterEnd = listened.end();
          while (iter != iterEnd) {
            SAFE_DELETE(*iter);
            iter++;
          }
        }
        {
          std::list<cEntry*>::iterator iter = loved.begin();
          const std::list<cEntry*>::iterator iterEnd = loved.end();
          while (iter != iterEnd) {
            SAFE_DELETE(*iter);
            iter++;
          }
        }
      }

      void cLastFM::Start(const string_t& _sKey, const string_t& _sSecret, const string_t& _sUserName, const string_t& _sPassword)
      {
        Stop();

        {
          util::cLockObject lock(mutex);
          sKey = _sKey;
          sSecret = _sSecret;
          sUserName = _sUserName;
          sPassword = _sPassword;
        }

        Run();
      }

      void cLastFM::StopSoon()
      {
        // Stop scrobbling the current song
        StopPlayingTrack();

        // Tell the thread to stop
        StopThreadSoon();
      }

      void cLastFM::Stop()
      {
        StopSoon();

        WaitToStop();
      }

      bool cLastFM::IsRunning() const
      {
        return util::cThread::IsRunning();
      }
    }
  }
}
