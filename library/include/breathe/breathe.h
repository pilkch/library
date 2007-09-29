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

#if defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__)
#error "This is a 64 bit compile, have fun!"
#define X86_64_SYSTEM   1
typedef int             int32;
typedef unsigned int    uint32;

typedef short           int16;
typedef unsigned short  uint16;
typedef char            int8;
typedef unsigned char   uint8;

/* 32 bit for comparison
typedef int             int32;
typedef unsigned int    uint32;

typedef short           int16;
typedef unsigned short  uint16;
typedef char            int8;
typedef unsigned char   uint8;*/
#endif

// Operating System
#if defined(WIN32) || defined(__WIN__) || \
	defined(_MSC_VER) || defined(__CYGWIN32__) || defined(_BORLANDC_) || defined(__MINGW32__)
#define PLATFORM_WINDOWS
#elif defined(__LINUX__) || defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__apple__) || defined(MACOSX) || defined (__MACOSX__)
#define PLATFORM_MAC
#elif defined(_XENON) || \
	defined(SN_TARGET_PSP_HW) || \
	defined(SN_TARGET_PS3)
#error "This platform has not been built on yet"
#else
#error "Need some help identifying the platform!"
#endif

// Architecture
#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MISPEB__)) || \
	(defined(__APPLE__) && defined(__MACH__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc__)
#error "We don't support big endian systems, get a real computer (x86)
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
#include <breathe/util/mem.h>


// Utility types, objects etc.

namespace breathe
{
#ifdef UNICODE
	typedef wchar_t unicode_char;
#ifndef PLATFORM_WINDOWS
	#define _TEXT(s) L##s
	#define TEXT(s) _TEXT(s)
#endif
#else
	typedef char unicode_char;
#ifndef PLATFORM_WINDOWS
	#define TEXT(s) s
#endif
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

	#define NO_COPY(T) \
		private: \
		T(const T&); \
		void operator=(const T&); \
		public: 
}

#endif //BREATHE_H