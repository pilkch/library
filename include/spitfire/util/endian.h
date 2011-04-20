#ifndef ENDIAN_H
#define ENDIAN_H

#include <spitfire/spitfire.h>

namespace spitfire
{
   namespace endian
   {
      #if defined(__WIN__) && !defined(COMPILER_GCC)
      // TODO: Are these required?
      #pragma intrinsic(_byteswap_ushort)
      #pragma intrinsic(_byteswap_ulong)
      #pragma intrinsic(_byteswap_uint64)

      inline uint16_t Reverse(uint16_t value) { return _byteswap_ushort(u); }
      inline uint32_t Reverse(uint32_t value) { return _byteswap_ulong(u); }
      inline uint64_t Reverse(uint64_t value) { return _byteswap_uint64(u); }
      #else
      inline uint16_t Reverse(uint16_t value) { return (((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8)); }
      inline uint32_t Reverse(uint32_t value) { return __builtin_bswap32(value); }
      inline uint64_t Reverse(uint64_t value) { return __builtin_bswap64(value); }
      #endif

      inline int16_t Reverse(int16_t value) { return Reverse(*(uint16_t*)&value); }
      inline int32_t Reverse(int32_t value) { return Reverse(*(uint32_t*)&value); }
      inline int64_t Reverse(int64_t value) { return Reverse(*(uint64_t*)&value); }

      #ifdef BUILD_ENDIAN_LITTLE
      // Little Endian
      template <class T> inline T LittleToNative(T value) { return value; }
      template <class T> inline T NativeToLittle(T value) { return value; }
      template <class T> inline T BigToNative(T value) { return Reverse(value); }
      template <class T> inline T NativeToBig(T value) { return Reverse(value); }
      #else
      // Big Endian
      template <class T> inline T LittleToNative(T value) { return Reverse(value); }
      template <class T> inline T NativeToLittle(T value) { return Reverse(value); }
      template <class T> inline T BigToNative(T value) { return value; }
      template <class T> inline T NativeToBig(T value) { return value; }
      #endif
   }
}

#endif // ENDIAN_H
