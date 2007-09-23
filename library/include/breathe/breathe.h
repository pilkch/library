/*************************************************************************
 *                                                                       *
 * Breathe Library, Copyright (C) 2006-2007 Chris Pilkington             *
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

#ifdef CMEM_H
!!! cMem.h already included, include cBreathe.h only
#endif //CMEM_H

#ifndef BREATHE_H
#define BREATHE_H

#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
!!! We don't support big endian systems, get a real computer (x86)
#endif

// Platform
#if defined(WIN32) || defined(_MSC_VER) || defined(_BORLANDC_) || defined(__WIN__)
#define PLATFORM_WINDOWS
#elif defined(__LINUX__) || defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined (__MACOSX__)
#define PLATFORM_MAC
#else
!!! We don't support this platform yet
#endif

// What type of build is this?
#ifdef NDEBUG
#define BUILD_RELEASE
#else
#define BUILD_DEBUG
#endif


// CRT's memory leak detection
#if defined BUILD_DEBUG && defined PLATFORM_WINDOWS
#include <crtdbg.h>
#endif


// For our types (uint8_t, uint32_t, etc.)
#include <SDL/SDL.h>


// FluidStudios' memory leak detection
#include <BREATHE/UTIL/cMem.h>


// Utility types, objects etc.

namespace BREATHE
{
#ifdef BUILD_UNICODE
	typedef wchar_t unicode_char;
#else
	typedef char unicode_char;
#endif

	// Constants
	const bool BAD=false;
	const bool GOOD=true;
	
	// Safe deleting functions
	template <class T>
	inline void SAFE_DELETE(T& x)
	{ 
		delete (x);
		(x)=NULL;
	}

	template <class T>
	inline void SAFE_DELETE_ARRAY(T& x)
	{ 
		delete [] (x);
		(x)=NULL;
	}
}

#endif //BREATHE_H