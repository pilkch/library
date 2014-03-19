#ifndef LIBWIN32MM_KEYS_H
#define LIBWIN32MM_KEYS_H

// Win32 headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  typedef uint32_t key_t;

  const key_t KEY_INVALID = 0;

  const key_t KEY_CONTROL = VK_CONTROL;
  const key_t KEY_SHIFT = VK_SHIFT;
  const key_t KEY_MENU = VK_MENU;
  const key_t KEY_ALT = VK_MENU;
  
  const key_t KEY_DELETE = VK_DELETE;
  const key_t KEY_INSERT = VK_INSERT;
  const key_t KEY_HOME = VK_HOME;
  const key_t KEY_END = VK_END;
  const key_t KEY_PGUP = VK_PRIOR;
  const key_t KEY_PGDN = VK_NEXT;
  const key_t KEY_PAUSE = VK_PAUSE;
  const key_t KEY_LEFT = VK_LEFT;
  const key_t KEY_RIGHT = VK_RIGHT;
  const key_t KEY_UP = VK_UP;
  const key_t KEY_DOWN = VK_DOWN;

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
