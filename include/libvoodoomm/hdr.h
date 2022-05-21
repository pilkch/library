#pragma once

// Standard headers
#include <string>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>

namespace voodoo
{

bool LoadHDR(const std::string& sFilePath, cImage& outImage);

}
