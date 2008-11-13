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

#ifndef BREATHE_H
#define BREATHE_H

#ifdef CMEM_H
#error "Don't include mem.h directly, include breathe.h instead"
#endif // CMEM_H

#ifdef FIRESTARTER
#define NO_SDL
#endif // FIRESTARTER

// Architecture
#if defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__) || defined(__LP64__) || defined(__amd64)
#error "This is a 64 bit compile, have fun!"
#endif

#if defined(__hppa__) || \
  defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
  (defined(__MIPS__) && defined(__MISPEB__)) || \
  (defined(__APPLE__) && defined(__MACH__)) || \
  defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
  defined(__sparc__)
#error "Breathe does not support big endian systems, get a real computer (little endian x86)
#endif

// Operating System
// Use the standard defines were possible:
// __WIN__, __LINUX__, __APPLE__
// as well as combination defines:
// PLATFORM_LINUX_OR_UNIX

#if defined(WIN32) || defined(__WIN__) || \
  defined(_MSC_VER) || defined(__CYGWIN32__) || defined(_BORLANDC_) || defined(__MINGW32__)
#ifndef __WIN__
#define __WIN__
#endif
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__LINUX__)
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

#if defined(__LINUX__) || defined(__APPLE__)
#define PLATFORM_LINUX_OR_UNIX
#endif

// What type of build is this?
#ifdef NDEBUG
#define BUILD_RELEASE
#else
#define BUILD_DEBUG
#endif

#if defined(min) || defined(max)
#error "For Visual Studio define NOMINMAX"
#endif

// CRT's memory leak detection
#ifdef __WIN__
#include <windows.h>

#ifdef BUILD_DEBUG
#include <crtdbg.h>
#ifndef __GNUC__
//#define _CRTDBG_MAP_ALLOC
inline void *__cdecl operator new(size_t n, const char *fn, int l) { return ::operator new(n, 1, fn, l); }
inline void __cdecl operator delete(void *p, const char *fn, int l) { ::operator delete(p, 1, fn, l); }
#define new new(__FILE__,__LINE__)
#endif
#endif

#if !defined(UNICODE) && !defined(_UNICODE)
#error "Add UNICODE and _UNICODE to the build flags in your IDE"
#endif // !UNICODE

#ifdef _MBCS
#error "_MBCS should not be defined by your IDE"
#endif

#elif defined(UNICODE)
#define _TEXT(s) L##s
#define TEXT(s) _TEXT(s)
#else
#define TEXT(s) s
#endif


// *** Types

// Now taken directly from math.h/cmath because of a name clash
// Warning on PLATFORM_LINUX_OR_UNIX this may be 80 bit floating-point (long double)
//#ifndef float_t
//typedef float float_t;
//#endif

#ifdef NO_SDL
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#else
// For our types (uint8_t, uint32_t, etc.)
#include <SDL/SDL.h>
#endif

typedef float float32_t; // For reading/writing to and from files/network
typedef double float64_t; // For reading/writing to and from files/network

#ifndef nullptr
#define nullptr NULL
#endif

#ifdef BUILD_DEBUG
#ifdef __WIN__
// *** FluidStudios' memory leak detection
#ifndef FIRESTARTER
#include <breathe/util/mem.h>
#endif
#endif
#endif // BUILD_DEBUG

#define NO_COPY(T) \
  private: \
  T(const T&); \
  void operator=(const T&)

// Utility types, objects etc.
namespace breathe
{
  // Types
  typedef uint32_t sampletime_t;

  #ifdef __WIN__
  #define SIZEOF_WCHAR_T 2
  typedef wchar_t char16_t;
  typedef uint32_t char32_t;
  #else
  #define SIZEOF_WCHAR_T 4
  typedef uint16_t char16_t;
  typedef wchar_t char32_t;
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

#ifndef NDEBUG
  void InformativeAssert(bool b, const char* szAssert, const char* szFile, int line, const char* szFunction);
#endif
}

#ifdef ASSERT
#undef ASSERT
#endif

#ifndef NDEBUG

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define ASSERT(p) breathe::InformativeAssert(p, #p, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#else
#define ASSERT(...)
#endif

#endif // BREATHE_H
