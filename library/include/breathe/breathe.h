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
#error "Don't include mem.h directly, include breathe.h instead"
#endif //CMEM_H

#ifndef BREATHE_H
#define BREATHE_H

#ifdef FIRESTARTER
#define NO_SDL
#endif

#if defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__) || defined(__LP64__)
#error "This is a 64 bit compile, have fun!"
#endif


// Operating System
// Use the standard defines were possible:
// __WIN__, __LINUX__, __APPLE__

#if defined(WIN32) || defined(__WIN__) || \
	defined(_MSC_VER) || defined(__CYGWIN32__) || defined(_BORLANDC_) || defined(__MINGW32__)
#ifndef __WIN__
#define __WIN__
#endif
#elif defined(__LINUX__) || defined(__linux__)
#ifndef __LINUX__
#define __LINUX__
#endif
#elif defined(__APPLE__) || defined(__apple__) || defined(MACOSX) || defined(__MACOSX__)
#ifndef __APPLE__
#define __APPLE__
#endif
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
#error "We don't support big endian systems, get a real computer (little endian x86)
#endif

// What type of build is this?
#ifdef NDEBUG
#define BUILD_RELEASE
#else
#define BUILD_DEBUG
#endif


// CRT's memory leak detection
#ifdef __WIN__
#include <windows.h>

#ifdef BUILD_DEBUG
#include <crtdbg.h>
#endif

#elif defined(UNICODE)
#define _TEXT(s) L##s
#define TEXT(s) _TEXT(s)
#else
#define TEXT(s) s
#endif


// *** Types
#ifdef NO_SDL
typedef unsigned char uint8_t;
typedef long int uint32_t;
#else
// For our types (uint8_t, uint32_t, etc.)
#include <SDL/SDL.h>
#endif

#ifndef nullptr
#define nullptr NULL
#endif


// *** FluidStudios' memory leak detection
#ifndef FIRESTARTER
#include <breathe/util/mem.h>
#endif


#define NO_COPY(T) \
	private: \
	T(const T&); \
	void operator=(const T&);


// Utility types, objects etc.
namespace breathe
{
	// Types
	typedef uint32_t sampletime_t;

	#ifdef __WIN__
	#define SIZEOF_WCHAR_T 2
	#else
	#define SIZEOF_WCHAR_T 4
	#endif

	#ifdef UNICODE
	typedef wchar_t char_t;
	#else
	typedef char char_t;
	#endif


	// Constants
	const bool BAD = false;
	const bool GOOD = true;
	

	// Safe deleting functions
	template <class T>
	inline void SAFE_DELETE(T& x)
	{ 
		delete x;
		x = NULL;
	}

	template <class T>
	inline void SAFE_DELETE_ARRAY(T& x)
	{ 
		delete [] x;
		x = NULL;
	}
}

#endif //BREATHE_H