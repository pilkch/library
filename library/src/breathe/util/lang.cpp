#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif

#include <breathe/util/lang.h>

namespace breathe
{
	namespace util
	{
		std::wstring LANG(const std::string& tag)
		{
			return TEXT("");
		}

		std::wstring LANG(const std::wstring& tag)
		{
			return TEXT("");
		}
	}
}
