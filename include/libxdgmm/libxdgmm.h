/*************************************************************************
 *                                                                       *
 * libxdgmm Library, Copyright (C) 2009 Onwards Chris Pilkington         *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

#ifndef LIBXDGMM_H
#define LIBXDGMM_H

// This is a simple wrapper around the xdg executables such as xdg-open, xdg-user-dir, etc.
// http://standards.freedesktop.org/basedir-spec/latest/index.html
// http://ploum.frimouvy.org/?207-modify-your-application-to-use-xdg-folders
// TODO: Also wrap desktop-file-utils, xdg-desktop-menu and xdg-desktop-icon
// Other possible utilities to wrap are xdg-screensaver and xdg-mime
// TODO: $XDG_DATA_DIRS, $XDG_CONFIG_DIRS, $XDG_CACHE_HOME have not been wrapped yet

#include <string>

// libxdg-basedir includes
#include <basedir.h>

namespace xdg
{
  // https://launchpad.net/ubuntu/+source/libxdg-basedir
  // http://n.ethz.ch/~nevillm/download/libxdg-basedir/doc/basedir_8h.html
  class cXdg
  {
  public:
    cXdg();
    ~cXdg();

    // Checks if the handle to the library has been opened successfully
    bool IsValid() const;

    // "The base directory relative to which user specific data files should be stored"
    // "/home/chris/"
    std::string GetHomeDirectory();

    // "The base directory relative to which user specific data files should be stored"
    // "/home/chris/.local/share/"
    std::string GetHomeDataDirectory();

    // "The base directory relative to which user specific configuration files should be stored"
    // "/home/chris/.config/"
    std::string GetHomeConfigDirectory();

    // User desktop folder
    // "/home/chris/Desktop/"
    std::string GetHomeDesktopDirectory();

    // User documents folder
    // "/home/chris/Documents/"
    std::string GetHomeDocumentsDirectory();

    // User downloads folder
    // "/home/chris/Download/"
    std::string GetHomeDownloadsDirectory();

    // User music folder
    // "/home/chris/Music/"
    std::string GetHomeMusicDirectory();

    // User pictures folder
    // "/home/chris/Pictures/"
    std::string GetHomePicturesDirectory();

    // User videos folder
    // "/home/chris/Videos/"
    std::string GetHomeVideosDirectory();

    // "Base directory for user specific non-essential data files"
    // "/home/chris/.cache/"
    std::string GetHomeTempDirectory();


    // Opening files, folders and URLs
    std::string GetOpenErrorString(int result);

    int OpenFile(const std::string& sFilePathUTF8);
    int OpenFolder(const std::string& sFolderPathUTF8);
    int OpenURL(const std::string& sURLUTF8);

  private:
    std::string GetDirectory(const std::string& sTag) const;

    bool bIsValid;
    xdgHandle handle;

    std::string home;
  };
}

#endif // LIBXDGMM_H
