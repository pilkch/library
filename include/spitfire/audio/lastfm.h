#ifndef SPITFIRE_LASTFM_H
#define SPITFIRE_LASTFM_H

// This is a simple wrapper around libclastfm
// http://liblastfm.sourceforge.net/

// Standard headers
#include <string>
#include <list>

// libclastfm headers
extern "C"{
#include <clastfm.h>
}

// Spitfire headers
#include <spitfire/algorithm/algorithm.h>
#include <spitfire/audio/metadata.h>
#include <spitfire/util/datetime.h>
#include <spitfire/util/mutex.h>
#include <spitfire/util/queue.h>
#include <spitfire/util/thread.h>

// Define these in your cmake file
//#define BUILD_LASTFM_KEY "<mykey>"
//#define BUILD_LASTFM_SECRET "<mysecret>"

namespace spitfire
{
  namespace audio
  {
    namespace lastfm
    {
      // ** cSession
      //
      // http://www.last.fm/api/scrobbling
      //
      // Submission Rules
      // The track must be longer than 30 seconds.
      // And the track has been played for at least half its duration, or for 4 minutes (whichever occurs earlier.)
      //
      class cSession
      {
      public:
        cSession(const string_t& sKey, const string_t& sSecret);
        ~cSession();

        void Login(const string_t& sUserName, const string_t& sPassword);

        bool IsLoggedIn() const;

        bool Scrobble(const cMetaData& metaData);
        bool UpdateTrack(const cMetaData& metaData);

        bool LoveTrack(const cMetaData& metaData);

      private:
        bool ScrobbleOrUpdateTrack(const cMetaData& metaData, bool bScrobble);

        LASTFM_SESSION* pSession;
        bool bIsLoggedIn;
      };


      // ** cEntry

      class cEntry
      {
      public:
        cMetaData metaData;
        util::cDateTime dateTime;
      };


      // ** cEvent

      class cEvent
      {
      public:
        enum class TYPE {
          START_TRACK,
          STOP_TRACK,
          LOVE_TRACK
        };
        TYPE type;

        cMetaData metaData;
        util::cDateTime dateTime;
      };


      // ** cLastFMErrorHandler

      class cLastFMErrorHandler
      {
      public:
        virtual ~cLastFMErrorHandler() {}

        virtual void OnLastFMErrorUserNameOrPasswordIncorrect() = 0;
        virtual void OnLastFMTracksQueuedUpdated(size_t nTracksQueued) = 0;
        virtual void OnLastFMTracksSubmitted(size_t nTracksSubmitted, const cMetaData& metaDataLastTrack, const util::cDateTime& dateTime) = 0;
      };


      // ** cLastFM

      class cLastFM : protected util::cThread
      {
      public:
        explicit cLastFM(cLastFMErrorHandler& handler);
        ~cLastFM();

        void Start(const string_t& sKey, const string_t& sSecret, const string_t& sUserName, const string_t& sPassword);
        void StopSoon();
        void Stop();

        bool IsRunning() const;

        constant_stack<cEntry> GetHistoryListened() const;
        constant_stack<cEntry> GetHistoryLoved() const;

        void StartPlayingTrack(const cMetaData& metaData);
        void StopPlayingTrack();
        void LoveTrack(const cMetaData& metaData);

      private:
        virtual void ThreadFunction() override;

        string_t GetListenedFile() const;
        void ReadListenedFromFile(std::list<cEntry*>& listened);
        void WriteListenedToFile(const std::list<cEntry*>& listened) const;

        string_t GetLovedFile() const;
        void ReadLovedFromFile(std::list<cEntry*>& loved);
        void WriteLovedToFile(const std::list<cEntry*>& loved) const;

        cLastFMErrorHandler& handler;

        string_t sKey;
        string_t sSecret;
        string_t sUserName;
        string_t sPassword;

        mutable util::cMutex mutex;
        constant_stack<cEntry> historyListened;
        constant_stack<cEntry> historyLoved;

        util::cSignalObject soAction;

        util::cThreadSafeQueue<cEvent> eventQueue;
      };

      inline cLastFM::cLastFM(cLastFMErrorHandler& _handler) :
        util::cThread(soAction, "cLastFM"),
        handler(_handler),
        mutex("cLastFM_mutex"),
        historyListened(10),
        historyLoved(10),
        soAction("cLastFM_soAction"),
        eventQueue(soAction)
      {
      }
    }
  }
}

#endif // SPITFIRE_LASTFM_H
