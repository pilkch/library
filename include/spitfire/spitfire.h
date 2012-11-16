/*************************************************************************
 *                                                                       *
 * Spitfire Library, Copyright (C) 2006 Onwards Chris Pilkington         *
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

// Defines
// BUILD_HTML_LOG - Allow logging to HTML
// BUILD_UNITTESTS - Build spitfire unit tests
// BUILD_SDL - Build with SDL support
// NDEBUG - Release build (Defaults to debug build otherwise)

#ifndef SPITFIRE_H
#define SPITFIRE_H

//#ifndef SPITFIRE_APPLICATION_NAME
//#error "SPITFIRE_APPLICATION_NAME must be defined"
//#endif

//#ifndef SPITFIRE_APPLICATION_NAME_LWR
//#error "SPITFIRE_APPLICATION_NAME_LWR must be defined"
//#endif

#ifdef CMEM_H
#error "Don't include mem.h directly, include spitfire.h instead"
#endif // CMEM_H

#ifndef BUILD_SDL
#define NO_SDL
#endif // !BUILD_SDL

// Architecture
#if defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__) || defined(__LP64__) || defined(__amd64)
#define BUILD_PLATFORM_64
#endif

#if defined(__hppa__) || \
  defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
  (defined(__MIPS__) && defined(__MISPEB__)) || \
  (defined(__APPLE__) && defined(__MACH__)) || \
  defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
  defined(__sparc__)
#warning "Building big endian, this will probably be broken in many, many places"
#define BUILD_ENDIAN_BIG
#else
#define BUILD_ENDIAN_LITTLE
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

#ifndef NOMINMAX
#error "For Visual Studio define NOMINMAX"
#endif

#ifdef _MSC_VER
#define COMPILER_MSVC
#elif defined(__GNUC__)
#define COMPILER_GCC
#else
#error "Unknown compiler"
#endif

#ifdef __WIN__
#include <windows.h>

#ifdef BUILD_DEBUG
// CRT's memory leak detection
#include <crtdbg.h>
#ifndef __GNUC__
//#define _CRTDBG_MAP_ALLOC
inline void *__cdecl operator new(size_t n, const char *fn, int l) { return ::operator new(n, 1, fn, l); }
inline void __cdecl operator delete(void *p, const char *fn, int l) { ::operator delete(p, 1, fn, l); }
#define new new(__FILE__,__LINE__)
#endif
#endif

#ifdef _MBCS
#error "_MBCS should not be defined by your IDE"
#endif
#ifdef _CPPUNWIND
#error "Exceptions must be disabled"
#endif
#ifdef _CPPRTTI
#error "RTTI must be disabled"
#endif

#ifndef UNICODE
#error "UNICODE must be defined on Windows"
#endif

#else

#ifdef UNICODE
#error "UNICODE must not be defined on this platform"
#endif

#define TEXT(s) s
#endif

#ifndef STRINGIFY
#define _STRINGIFY(number) #number
#define STRINGIFY(number)  _STRINGIFY(number)
#endif


// Thread local storage
// NOTE: This is only for POD types
// http://en.wikipedia.org/wiki/Thread-local_storage

// Again Visual Studio does it's own thing
#ifdef COMPILER_MSVC
#define THREAD_LOCAL __declspec( thread )
#else
#define THREAD_LOCAL __thread
#endif

// This is basically for documenting that a static/global variable is shared between threads (It shouldn't be used much at all, THREAD_LOCAL should basically be the default)
#define THREAD_GLOBAL


#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 100) + (__GNUC_MINOR__)
#define GCC_VERSION_4_7 407
#endif

#ifdef __GNUC__
// Override keyword to flag virtual functions which are overridden from the base class
#define override
// Final keyword to flag virtual functions which are not allowed to be overridden in derived classes
#define final
#endif

#ifdef COMPILER_MSVC
#undef interface
// Avoid problems with using the interface keyword
#define interface Interface

// Make sure that we don't try to use C++11 functionality
//#define BUILD_SPITFIRE_NO_CPP11
#endif

#ifdef BUILD_SPITFIRE_NO_CPP11
// Static compile time assert
#define static_assert assert
#endif

// Deprecation flags
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define ATTRIBUTE_DEPRECATED __attribute__((deprecated))
#else
#define ATTRIBUTE_DEPRECATED
#endif


// *** Types

#ifdef NO_SDL
#include <cstdint>
#include <cstddef>
#else
// For our types (uint8_t, uint32_t, etc.)
#include <SDL/SDL.h>
#ifdef BUILD_PLATFORM_64
#include <stdint.h>
#endif
#endif

typedef float float32_t; // For reading/writing to and from files/network
typedef double float64_t; // For reading/writing to and from files/network

// Warning on PLATFORM_LINUX_OR_UNIX this may be 80 bit floating-point (long double)
#ifdef COMPILER_MSVC
typedef float float_t;
#endif

// Apparently Visual Studio doesn't have ssize_t
#ifdef COMPILER_MSVC
#ifdef BUILD_PLATFORM_64
typedef int64_t ssize_t;
#else
typedef int32_t ssize_t;
#endif
#endif

#ifdef BUILD_DEBUG
#ifdef COMPILER_MSVC
// *** FluidStudios' memory leak detection
#ifndef FIRESTARTER
//#include <spitfire/util/mem.h>
#endif
#endif
#endif // BUILD_DEBUG

#define NO_COPY(T) \
  private: \
  T(const T&); \
  void operator=(const T&)


// Like sizeof for arrays (Although using raw arrays like this is discouraged so we can probably remove this
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define countof(array) (sizeof(ArraySizeHelper(array)))


// Utility types, objects etc.
namespace spitfire
{
  // For representing a file size on both 32 bit and 64 bit architectures
  typedef uint64_t filesize_t;

  // For specifying a time duration
  typedef uint64_t sampletime_t; // Legacy
  typedef uint64_t durationms_t;
}


// Safe deleting functions
// These will delete your object/array and set your pointer to NULL so that we avoid double deletes
namespace spitfire
{
  template <class T>
  inline void SAFE_DELETE(T& x)
  {
    delete x;
    x = nullptr;
  }

  template <class T>
  inline void SAFE_DELETE_ARRAY(T& x)
  {
    delete [] x;
    x = nullptr;
  }
}


// Static Assert
// NOTE: These are compile time so we do them in both debug and release
#define STATIC_ASSERT(expression, szDescription) static_assert(expression, szDescription)


// Logging and assert
#if defined(COMPILER_MSVC) || defined(BUILD_DEBUG)
#define BUILD_LOGGING
// LOG and LOGERROR are declared in spitfire/util/log.h
#else
#define LOG std::cout
#define LOGERROR std::cerr
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef BUILD_DEBUG

#ifdef BUILD_HTML_LOG

// Assert
namespace spitfire
{
  void InformativeAssert(bool b, const char* szAssert, const char* szFile, const char* szFunction, size_t line);
}

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define ASSERT(p) spitfire::InformativeAssert(p, #p, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#else
#define ASSERT assert
#endif

#else
#define ASSERT(...)
#endif

#endif // SPITFIRE_H
