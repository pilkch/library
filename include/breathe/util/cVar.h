#ifndef CVAR_H
#define CVAR_H

namespace breathe
{
  //  cVar
  //
  //  name=The name of the cvar.  For example "sv_gravity".
  //  value=The value of the cvar.  For example "800.0".
  //  flags=The flags that specify how the cvar acts.  For example VAR_CLIENT | VAR_SERVER means that it will be saved to the client and server config files.
  //

  class cVar
  {
  public:
    // Default 0 is session only (Not saved at all), read and write at any time
    enum FLAGS {
      FLAGS_NULL = 0,

      FLAGS_SAVE_CLIENT = 1,          // Written to client.xml
      FLAGS_SAVE_SERVER = 2,          // Written to server.xml

      FLAGS_READ_ONLY_IN_GAME = 4,    // Can't change once in game.  For example: "sv_maxplayers" etc.
      FLAGS_READ_ONLY_IN_CONSOLE = 8, // Can't change once in console.  For example: "r_driver" etc.
      FLAGS_READ_ONLY = 16            // Can't change at all ever, even programatically.  Used for functions.  For example: "help", "version" etc.
    };

    cVar();
    cVar(const cVar& rhs);

    // Note: Implicit conversion from these types is ok
    cVar(bool rhs) { SetValue(rhs); }
    cVar(int rhs) { SetValue(rhs); }
    cVar(float rhs) { SetValue(rhs); }
    cVar(const char_t* rhs) { SetValue(rhs); }
    cVar(const string_t& rhs) { SetValue(rhs); }

    cVar& operator=(const cVar& rhs);
    cVar& operator=(bool rhs) { SetValue(rhs); return *this; }
    cVar& operator=(int rhs) { SetValue(rhs); return *this; }
    cVar& operator=(float rhs) { SetValue(rhs); return *this; }
    cVar& operator=(const char_t* rhs) { SetValue(rhs); return *this; }
    cVar& operator=(const string_t& rhs) { SetValue(rhs); return *this; }

    bool SetValue(bool value);
    bool SetValue(int value);
    bool SetValue(float value);
    bool SetValue(const char_t* value);
    bool SetValue(const string_t& value);

    bool GetBool() const { return bValue; }
    int GetInt() const { return iValue; }
    float GetFloat() const { return fValue; }
    const string_t& GetString() const { return sValue; }

    bool IsReadOnlyAtTheMoment() const;

  private:
    FLAGS flags;

    bool bValue;
    int iValue;
    float fValue;
    string_t sValue;
  };

  inline cVar::cVar() :
    flags(FLAGS_NULL)
  {
    SetValue(true);
  }

  inline cVar::cVar(const cVar& rhs)
  {
    flags = rhs.flags;
    bValue = rhs.bValue;
    iValue = rhs.iValue;
    fValue = rhs.fValue;
    sValue = rhs.sValue;
  }

  inline cVar& cVar::operator=(const cVar& rhs)
  {
    flags = rhs.flags;
    bValue = rhs.bValue;
    iValue = rhs.iValue;
    fValue = rhs.fValue;
    sValue = rhs.sValue;

    return *this;
  }

  inline bool cVar::SetValue(bool value)
  {
    if (IsReadOnlyAtTheMoment()) return false;

    bValue = value;
    if (bValue) {
      iValue = 1;
      fValue = 1.0f;
      sValue = TEXT("true");
    } else {
      iValue = 0;
      fValue = 0.0f;
      sValue = TEXT("false");
    }

    return true;
  }

  inline bool cVar::SetValue(int value)
  {
    if (IsReadOnlyAtTheMoment()) return false;

    bValue = bool(value);
    iValue = value;
    fValue = float(value);
    sValue = breathe::string::ToString(value);

    return true;
  }

  inline bool cVar::SetValue(float value)
  {
    if (IsReadOnlyAtTheMoment()) return false;

    bValue = bool(value);
    iValue = int(value);
    fValue = value;
    sValue = breathe::string::ToString(value);

    return true;
  }

  inline bool cVar::SetValue(const char_t* value)
  {
    const string_t sValueTemp(value);
    return SetValue(sValueTemp);
  }

  inline bool cVar::SetValue(const string_t& value)
  {
    if (IsReadOnlyAtTheMoment()) return false;

    bValue = breathe::string::ToBool(value);
    iValue = breathe::string::ToInt(value);
    fValue = breathe::string::ToFloat(value);
    sValue = value;

    return true;
  }

  inline bool cVar::IsReadOnlyAtTheMoment() const
  {
    if (flags & FLAGS_READ_ONLY) return true;
    //if ((flags & FLAGS_READ_ONLY_IN_CONSOLE) && pConsole->IsInConsole()) return true;
    //if ((flags & FLAGS_READ_ONLY_IN_GAME) && pConsole->IsInGame()) return true;

    return false;
  }

  class var //cVarRegistry
  {
  public:
    template <class T>
    static void VarSet(const string_t& name, const T& value)
    {
      cVar* pVar = VarFind(name);
      if (pVar != nullptr) {
        pVar->SetValue(value);
        return;
      }

      mVar[name] = new cVar(value);
    }

    static void VarSet(const string_t& name, const char_t* value)
    {
      cVar* pVar = VarFind(name);
      if (pVar != nullptr) {
        pVar->SetValue(value);
        return;
      }

      mVar[name] = new cVar(value);
    }

    static cVar* VarFind(const string_t& name);
    static void PrintAll();

  private:
    static std::map<string_t, cVar*> mVar;
  };
}

#endif // CVAR_H
