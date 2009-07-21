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

namespace xdg
{
  // For checking if xdg executables are actually present
  bool IsInstalled();


  // "The base directory relative to which user specific data files should be stored"
  void GetDataHome(std::string& directory);

  // "The base directory relative to which user specific configuration files should be stored"
  void GetConfigHome(std::string& directory);


  // Opening files, folders and URLs
  std::string GetOpenErrorString(int result);

  int OpenFile(const std::string& file);
  int OpenFolder(const std::string& folder);
  int OpenURL(const std::string& url);
}

#endif // LIBXDGMM_H
