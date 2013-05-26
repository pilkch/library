// Standard headers
#include <cstring>
#include <iostream>
#include <sstream>

// Spitfire headers
#include <spitfire/audio/playlist.h>
#include <spitfire/storage/filesystem.h>

namespace spitfire
{
  namespace audio
  {
    // ** cTrack

    cTrack::cTrack() :
      uiTrackLengthMS(0)
    {
    }


    // ** cPlaylist

    cPlaylist::~cPlaylist()
    {
      Clear();
    }

    void cPlaylist::Clear()
    {
      const size_t n = tracks.size();
      for (size_t i = 0; i < n; i++) {
        SAFE_DELETE(tracks[i]);
      }
    }


    // ** cPlayListReader

    bool cPlayListReader::IsPlayListFile(const string_t& sFilePath) const
    {
      std::cout<<"cPlayListReader::IsPlayListFile \""<<spitfire::string::ToUTF8(sFilePath)<<"\""<<std::endl;

      string_t sExtension = filesystem::GetExtension(sFilePath);
      sExtension = spitfire::string::ToLower(sExtension);

      return ((sExtension == TEXT("pls")) || (sExtension == TEXT("m3u")));
    }

    bool cPlayListReader::Load(const string_t& sFilePath, cPlaylist& playlist) const
    {
      std::cout<<"cPlayListReader::Load \""<<spitfire::string::ToUTF8(sFilePath)<<"\""<<std::endl;

      string_t sExtension = filesystem::GetExtension(sFilePath);
      sExtension = spitfire::string::ToLower(sExtension);

      if (sExtension == TEXT("pls")) return LoadFromPls(sFilePath, playlist);
      else if (sExtension == TEXT("m3u")) return LoadFromM3u(sFilePath, playlist);

      // Unknown format
      return false;
    }

    bool cPlayListReader::LoadFromM3u(const string_t& sM3uPath, cPlaylist& playlist) const
    {
      std::cout<<"cPlayListReader::LoadFromM3u \""<<spitfire::string::ToUTF8(sM3uPath)<<"\""<<std::endl;

      playlist.tracks.clear();

      storage::cReadTextFile file(sM3uPath);

      const string_t sPlaylistPath = filesystem::GetPath(sM3uPath);

      cTrack track;

      // Fill the list with file paths
      string_t sLine;
      while (file.ReadLine(sLine)) {
        // #EXTINF:123, Sample artist - Sample title
        // Path/To/Song/Sample artist - Sample title.mp3

        sLine = string::Trim(sLine); // Get rid of any whitespace at the start and end

        string::cStringParser sp(sLine);

        // Skip blank lines
        if (sp.IsEnd()) continue;

        // Skip header
        if (sp.SkipToStringAndSkip(TEXT("#EXTM3U"))) continue;

        // Parse an extended info entry
        if (sp.SkipToStringAndSkip(TEXT("#EXTINF:"))) {
          // Length
          string_t sTrackLengthSeconds;
          sp.GetToStringAndSkip(TEXT(","), sTrackLengthSeconds);
          track.uiTrackLengthMS = string::ToUnsignedInt(sTrackLengthSeconds) * 1000;

          sp.SkipWhiteSpace();

          // Artist
          sp.GetToStringAndSkip(TEXT("-"), track.sArtist);
          string::Trim(track.sArtist);

          // Title
          track.sTitle = sp.GetToEnd();
          string::Trim(track.sTitle);
        } else {
          // Parse a file path, get the absolute path
          track.sFullPath = filesystem::MakePathAbsolute(sPlaylistPath, sp.GetToEnd());

          // Add the track to the playlist
          cTrack* pTrack = new cTrack;
          *pTrack = track;
          playlist.tracks.push_back(pTrack);
        }
      }

      return true;
    }

    bool cPlayListReader::LoadFromPls(const string_t& sPlsPath, cPlaylist& playlist) const
    {
      std::cout<<"cPlayListReader::LoadFromPls \""<<spitfire::string::ToUTF8(sPlsPath)<<"\""<<std::endl;

      playlist.tracks.clear();

      // Get the version and number of entries
      unsigned int uiVersion = 0;
      size_t uiNumberOfEntries = 0;
      {
        std::cout<<"cPlayListReader::LoadFromPls Looking for version and number of entries"<<std::endl;
        storage::cReadTextFile file(sPlsPath);
        string_t sLine;
        while (file.ReadLine(sLine)) {
          std::cout<<"cPlayListReader::LoadFromPls Line \""<<sLine<<"\""<<std::endl;
          if (string::StartsWith(sLine, TEXT("Version="))) {
            uiVersion = string::ToUnsignedInt(sLine.substr(strlen("Version=")));
          } else if (string::StartsWith(sLine, TEXT("NumberOfEntries="))) {
            uiNumberOfEntries = string::ToUnsignedInt(sLine.substr(strlen("NumberOfEntries=")));
          }
        }
      }

      // If the version doesn't match then we cannot proceed
      if (uiVersion != 2) {
        std::cout<<"cPlayListReader::LoadFromPls Incorrect version found "<<uiVersion<<", expected version 2"<<std::endl;
        return false;
      }

      storage::cReadTextFile file(sPlsPath);

      const string_t sPlaylistPath = filesystem::GetPath(sPlsPath);

      playlist.tracks.reserve(uiNumberOfEntries);
      for (size_t i = 0; i < uiNumberOfEntries; i++) playlist.tracks.push_back(new cTrack);

      string_t sLine;
      string_t sItem;
      while (file.ReadLine(sLine)) {
        // [playlist]
        // File1=Alternative\everclear - SMFTA.mp3
        // Title1=Everclear - So Much For The Afterglow
        // Length1=233
        // File2=Comedy\01 - Weird Al - Everything You Know Is Wrong.mp3
        // Title2=Weird Al - Everything You Know Is Wrong
        // Length2=227
        // NumberOfEntries=2
        // Version=2

        sLine = string::Trim(sLine); //gets rid of any whitespace at the start and end

        string::cStringParser sp(sLine);

        // Skip blank lines
        if (sp.IsEnd()) continue;

        if (sp.SkipToStringAndSkip(TEXT("File"))) { // "File1"
          if (sp.GetToStringAndSkip(TEXT("="), sItem)) {
            const uint64_t item = string::ToUnsignedInt(sItem);
            std::cout<<"cPlayListReader::LoadFromPls File"<<item<<" "<<sItem<<std::endl;
            if (item != 0) {
              std::cout<<"cPlayListReader::LoadFromPls a"<<std::endl;
              const size_t index = item - 1;
              if (index < uiNumberOfEntries) {
                std::cout<<"cPlayListReader::LoadFromPls b"<<std::endl;
                cTrack* pTrack = playlist.tracks[index];
                pTrack->sFullPath = filesystem::MakePathAbsolute(sPlaylistPath, sp.GetToEnd());
              }
            }
          }
        } else if (sp.SkipToStringAndSkip(TEXT("Title"))) { // "Title1"
          if (sp.GetToStringAndSkip(TEXT("="), sItem)) {
            const uint64_t item = string::ToUnsignedInt(sItem);
            if (item != 0) {
              const size_t index = item - 1;
              if (index < uiNumberOfEntries) {
                cTrack* pTrack = playlist.tracks[index];

                // Artist
                sp.GetToStringAndSkip(TEXT("-"), pTrack->sArtist);
                string::Trim(pTrack->sArtist);

                // Title
                pTrack->sTitle = sp.GetToEnd();
                string::Trim(pTrack->sTitle);
              }
            }
          }
        } else if (sp.SkipToStringAndSkip(TEXT("Length"))) { // "Length1"
          if (sp.GetToStringAndSkip(TEXT("="), sItem)) {
            const uint64_t item = string::ToUnsignedInt(sItem);
            if (item != 0) {
              const size_t index = item - 1;
              if (index < uiNumberOfEntries) {
                cTrack* pTrack = playlist.tracks[index];

                // Length
                string_t sTrackLengthSeconds;
                sp.GetToStringAndSkip(TEXT(","), sTrackLengthSeconds);
                pTrack->uiTrackLengthMS = string::ToUnsignedInt(sTrackLengthSeconds) * 1000;
              }
            }
          }
        }
      }

      return true;
    }


    // ** cPlayListWriter

    bool cPlayListWriter::Save(const string_t& sFilePath, const cPlaylist& playlist) const
    {
      std::cout<<"cPlayListWriter::Save \""<<spitfire::string::ToUTF8(sFilePath)<<"\""<<std::endl;

      string_t sExtension = filesystem::GetExtension(sFilePath);

      sExtension = spitfire::string::ToLower(sExtension);

      if (sExtension == TEXT("pls")) return SaveToPls(sFilePath, playlist);
      else if (sExtension == TEXT("m3u")) return SaveToM3u(sFilePath, playlist);

      // Unknown playlist format
      std::cout<<"cPlayListWriter::Save unsupported format \""<<spitfire::string::ToUTF8(sExtension)<<"\""<<std::endl;
      return false;
    }

    bool cPlayListWriter::SaveToM3u(const string_t& sM3uPath, const cPlaylist& playlist) const
    {
      std::cout<<"cPlayListWriter::SaveToM3u \""<<spitfire::string::ToUTF8(sM3uPath)<<"\""<<std::endl;

      storage::cWriteFile file;
      if (!file.Open(sM3uPath)) {
        std::cerr<<"cPlayListWriter::SaveToM3u failed to open file for writing"<<std::endl;
        return false;
      }

      const string_t sM3uFolder = filesystem::GetPath(sM3uPath);

      ostringstream_t o;
      o<<TEXT("#EXTM3U\n"); // Extended m3u header

      std::vector<cTrack*>::const_iterator iter = playlist.tracks.begin();
      const std::vector<cTrack*>::const_iterator iterEnd = playlist.tracks.end();
      while (iter != iterEnd) {
        // #EXTINF:123, Sample artist - Sample title
        // Path/To/Song/Sample artist - Sample title.mp3

        const cTrack* pTrack = *iter;
        ASSERT(pTrack != nullptr);
        const cTrack& track = *pTrack;

        o<<TEXT("#EXTINF:"); //extended info

        // Length
        const uint64_t uiLengthInSeconds = track.uiTrackLengthMS / 1000;
        o<<uiLengthInSeconds<<TEXT(", ");

        // Title
        const string_t sFullTitle = GetTrackTitle(track);
        o<<sFullTitle<<TEXT("\n");

        // Write out the path to the file
        const string_t sRelPath = filesystem::MakePathRelative(sM3uFolder, track.sFullPath);
        o<<sRelPath<<TEXT("\n");

        iter++;
      }

      file.WriteStringUTF8(o.str());

      return true;
    }

    bool cPlayListWriter::SaveToPls(const string_t& sPlsPath, const cPlaylist& playlist) const
    {
      std::cout<<"cPlayListWriter::SaveToPls \""<<spitfire::string::ToUTF8(sPlsPath)<<"\""<<std::endl;

      storage::cWriteFile file;
      if (!file.Open(sPlsPath)) {
        std::cerr<<"cPlayListWriter::SaveToPls failed to open file for writing"<<std::endl;
        return false;
      }

      const string_t sPlsFolder = filesystem::GetPath(sPlsPath);

      ostringstream_t o;
      o<<TEXT("[playlist]\n"); // Pls header

      size_t i = 0;

      std::vector<cTrack*>::const_iterator iter = playlist.tracks.begin();
      const std::vector<cTrack*>::const_iterator iterEnd = playlist.tracks.end();
      while (iter != iterEnd) {
        // [playlist]
        // File1=Alternative\Everclear - So Much For The Afterglow.mp3
        // Title1=Everclear - So Much For The Afterglow
        // Length1=233
        // File2=Classical\Artist - Title.mp3
        // Title2=Artist - Title
        // Length2=227
        // NumberOfEntries=5
        // Version=2

        const cTrack* pTrack = *iter;
        ASSERT(pTrack != nullptr);
        const cTrack& track = *pTrack;

        int iIndex = int(i) + 1; // Index is 1 based

        // File "File1=Artist\song.mp3"
        const string_t sRelativePath = filesystem::MakePathRelative(sPlsFolder, track.sFullPath);
        o<<TEXT("File")<<iIndex<<"="<<sRelativePath<<"\n";

        // Title "Title1=Artist - song"
        const string_t sTitle = GetTrackTitle(track);
        o<<TEXT("Title")<<iIndex<<"="<<sTitle<<"\n";

        // Length "Length1=0"
        const uint64_t uiLengthInSeconds = track.uiTrackLengthMS / 1000;
        o<<TEXT("Length")<<iIndex<<"="<<uiLengthInSeconds<<TEXT("\n");

        i++;
        iter++;
      }

      // Total track count
      o<<TEXT("NumberOfEntries=")<<i<<TEXT("\n");

      // Version
      o<<TEXT("Version=2\n");

      // Write the string
      file.WriteStringUTF8(o.str());

      return true;
    }

    string_t cPlayListWriter::GetTrackTitle(const cTrack& track) const
    {
      std::cout<<"cPlayListWriter::GetTrackTitle"<<std::endl;

      string_t sFullTitle;

      const string_t sTagArtist = track.sArtist;
      const string_t sTagTitle = track.sTitle;
      if (!sTagArtist.empty() && !sTagTitle.empty()) {
        // Use the artist and title
        sFullTitle = sTagArtist + TEXT(" - ") + sTagTitle;
      } else {
        // Either "title" or "artist" is missing, use the file name
        sFullTitle = filesystem::GetFileNoExtension(track.sFullPath);
      }

      return sFullTitle;
    }
  }
}
