// Standard headers
#include <sstream>

// libwin32mm headers
#include <libwin32mm/keys.h>

namespace win32mm
{
  #define LANGTAG(x) TEXT(x)

  #define KEY_NAME_LENGTH 100

  #ifdef UNICODE
  #define tstrcpy wcscpy_s
  #else
  #define tstrcpy strcpy_s
  #endif


  string_t KeyGetDescription(key_t key)
  {
    const uint16_t uVirtKey = uint16_t(key);
    uint32_t uScanCode = ::MapVirtualKey(uVirtKey, 0);

    // Set the extended Key flag for keys which are apart of the extended keyboard
    // NOTE: There is a conflict for keys with the same function coming from the extended keyboard so we need to make sure that the flag is set
    const key_t keyNoMod = KeyRemoveModFlags(key);
    if ((keyNoMod == KEY_LEFT) || (keyNoMod == KEY_RIGHT) || (keyNoMod == KEY_UP) || (keyNoMod == KEY_DOWN) || (keyNoMod == KEY_INSERT) ||
      (keyNoMod == KEY_PGUP) || (keyNoMod == KEY_PGDN) || (keyNoMod == KEY_DELETE) || (keyNoMod == KEY_HOME) || (keyNoMod == KEY_END)) {
      uScanCode |= 0x100;
    }

    char_t szKeyName[KEY_NAME_LENGTH];
    szKeyName[0] = 0;

    ::GetKeyNameText(MAKELPARAM(uVirtKey, uScanCode), szKeyName, KEY_NAME_LENGTH);

    // Special case for the pause key
    if ((szKeyName[0] == 0) && (keyNoMod == KEY_PAUSE)) tstrcpy(szKeyName, LANGTAG("Pause"));

    // Build the key destription
    ostringstream_t o;
    if ((key & KEY_MOD_CONTROL) != 0) o<<LANGTAG("Ctrl") TEXT("+");
    if ((key & KEY_MOD_ALT) != 0) o<<LANGTAG("Alt") TEXT("+");
    if ((key & KEY_MOD_SHIFT) != 0) o<<LANGTAG("Shift") TEXT("+");
    o<<szKeyName;

    return o.str();
  }
}
