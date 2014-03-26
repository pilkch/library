#ifndef LIBWIN32MM_FILESYSTEM_H
#define LIBWIN32MM_FILESYSTEM_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  bool MoveFileOrFolderToRecycleBin(const string_t& sFileOrFolderPath);
}

#endif // LIBWIN32MM_FILESYSTEM_H
