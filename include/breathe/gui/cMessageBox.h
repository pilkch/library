#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include <breathe/gui/cWindow.h>

namespace breathe
{
  namespace gui
  {
    class cMessageBoxSettings
    {
    public:
      friend class MESSAGE_BOX;

      cMessageBoxSettings();

      void SetTitleAndText(const string_t& title, const string_t& text);

      void SetModal() { bIsModeless = false; }
      void SetModeless() { bIsModeless = true; }

      void SetTypeInformation() { type = TYPE_INFORMATION; }
      void SetTypeWarning() { type = TYPE_WARNING; }
      void SetTypeError() { type = TYPE_ERROR; }

      void SetButtonYesText(const string_t& text);
      void AddButtonNoWithText(const string_t& text);
      void AddButtonCancel();

    protected:
      const string_t& GetTitle() const { return title; }
      const string_t& GetText() const { return text; }
      bool IsModeless() const { return bIsModeless; }
      TYPE GetType() const { return type; }

      string_t GetYesButtonText() const { return buttonTextYes; }
      bool HasNoButton() const { return bHasButtonNo; }
      string_t GetNoButtonText() const { return buttonTextNo; }
      bool HasCancelButton() const { return bHasButtonCancel; }

    private:
      enum class TYPE {
        INFORMATION,
        WARNING,
        ERROR
      };

      bool bIsModeless;
      TYPE type;
      string_t title;
      string_t text;

      // Every message box has a yes button
      string_t buttonTextYes;

      bool bHasButtonNo;
      string_t buttonTextNo;

      bool bHasButtonCancel;
      // Every cancel button has the default L__Cancel text, not customisable
    };

    cMessageBoxSettings::cMessageBoxSettings()
    {
      SetModal();
      SetTypeInformation();
      SetTitleAndText(TEXT("MessageBox"), TEXT("Text"));
      SetButtonYesText(LANG("L__Yes"));
    }

    void cMessageBoxSettings::SetTitleAndText(const string_t& newTitle, const string_t& newText)
    {
      title = newTitle;
      text = newText;
    }

    void cMessageBoxSettings::SetButtonYesText(const string_t& text)
    {
      buttonTextYes = text;
    }

    void cMessageBoxSettings::AddButtonNoWithText(const string_t& text)
    {
      bHasButtonNo = true;
      buttonTextNo = text;
    }

    void cMessageBoxSettings::AddButtonCancel()
    {
      bHasButtonCancel = true;
    }

    class cMessageBoxListener
    {
    public:
      void OnMessageBoxReturnedYes() { _OnMessageBoxReturn(MESSAGE_BOX_RESULT_YES); }
      void OnMessageBoxReturnedNo() { _OnMessageBoxReturn(MESSAGE_BOX_RESULT_NO); }
      void OnMessageBoxReturnedCancel() { _OnMessageBoxReturn(MESSAGE_BOX_RESULT_CANCEL); }

    private:
      enum class MESSAGE_BOX_RESULT {
        RESULT_YES,
        RESULT_NO,
        RESULT_CANCEL
      };

      virtual void _OnMessageBoxReturn(MESSAGE_BOX_RESULT result) = 0;
    };


    class MESSAGE_BOX
    {
    public:
      static void OpenMessageBox(const cMessageBoxSettings& settings, cMessageBoxListener& listener);

    private:
      // No instantiation
      MESSAGE_BOX();
    };
  }
}

#endif // CMESSAGEBOX_H
