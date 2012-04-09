/*************************************************************************
 *                                                                       *
 * libtrashmm Library, Copyright (C) 2012 Onwards Chris Pilkington       *
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

#ifndef LIBTRASHMM_H
#define LIBTRASHMM_H

// http://freedesktop.org/wiki/Specifications/trash-spec
// http://www.ramendik.ru/docs/trashspec.html
// https://github.com/andreafrancia/trash-cli/blob/master/trashcli/trash.py
// https://github.com/nesono/nesono-bin/blob/master/bashtils/rm2trash.linux

#include <string>

namespace trash
{
  enum class RESULT {
    ERROR_FILE_DOES_NOT_EXIST,
    SUCCESS
  };
  RESULT MoveFileToTrash(const std::string& sFilePath);
  RESULT MoveFolderToTrash(const std::string& sFolderPath);
}

#endif // LIBTRASHMM_H
