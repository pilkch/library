#ifndef LIBWIN32MM_KEYS_H
#define LIBWIN32MM_KEYS_H

// Win32 headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  typedef uint32_t key_t;

  #define KEY_INVALID win32mm::key_t(0)

  #define KEY_CONTROL win32mm::key_t(VK_CONTROL)
  #define KEY_SHIFT win32mm::key_t(VK_SHIFT)
  #define KEY_MENU win32mm::key_t(VK_MENU)
  #define KEY_ALT win32mm::key_t(VK_MENU)
  
  #define KEY_DELETE win32mm::key_t(VK_DELETE)
  #define KEY_INSERT win32mm::key_t(VK_INSERT)
  #define KEY_HOME win32mm::key_t(VK_HOME)
  #define KEY_END win32mm::key_t(VK_END)
  #define KEY_PGUP win32mm::key_t(VK_PRIOR)
  #define KEY_PGDN win32mm::key_t(VK_NEXT)
  #define KEY_PAUSE win32mm::key_t(VK_PAUSE)
  #define KEY_LEFT win32mm::key_t(VK_LEFT)
  #define KEY_RIGHT win32mm::key_t(VK_RIGHT)
  #define KEY_UP win32mm::key_t(VK_UP)
  #define KEY_DOWN win32mm::key_t(VK_DOWN)

  // These are the MOD_ keys shifted 16 bits for values see winuser.h
  #define KEY_MOD_CONTROL win32mm::key_t(0x00020000)
  #define KEY_MOD_ALT win32mm::key_t(0x00010000)
  #define KEY_MOD_SHIFT win32mm::key_t(0x00040000)

  #define KEY_COMBO_CONTROL(f) win32mm::key_t(KEY_MOD_CONTROL | f)
  #define KEY_COMBO_ALT(f) win32mm::key_t(KEY_MOD_ALT | f)
  #define KEY_COMBO_SHIFT(f) win32mm::key_t(KEY_MOD_SHIFT | f)
  #define KEY_COMBO_CONTROLSHIFT(f) win32mm::key_t(KEY_MOD_CONTROL | KEY_MOD_SHIFT | f)
  #define KEY_COMBO_ALTSHIFT(f) win32mm::key_t(KEY_MOD_ALT | KEY_MOD_SHIFT | f)


  string_t KeyGetDescription(key_t key);

  // Remove position specific information (LSHIFT/RSHIFT, LCTRL/RCTRL, LALT/RALT) leaving the key (KEY_SHIFT, KEY_CONTROL, KEY_ALT)
  inline key_t KeyRemoveModFlags(key_t key)
  {
    return (key & 0x0000ffff);
  }
}

#endif // LIBWIN32MM_KEYS_H
