// Standard headers
#include <string>

// libwin32mm headers
#include <libwin32mm/aboutdialog.h>
#include <libwin32mm/bitmap.h>
#include <libwin32mm/controls.h>
#include <libwin32mm/dialog.h>

// Spitfire headers
#include <spitfire/math/math.h>
#include <spitfire/util/datetime.h>

namespace win32mm
{
  const int ID_ABOUTDIALOG_LINKCONTROL = 101;

  class cAboutDialog : public cDialog, public cLinkControlListener
  {
  public:
    bool Run(cWindow& parent);

  private:
    virtual void OnInit() override;
    virtual bool OnOk() override;

    virtual void OnLinkClicked(const cLinkControl& linkControl, size_t index);

    void LayoutAndSetWindowSize();
    void EnableControls();

    cImageControl imageControl;

    cLinkControl linkControl;

    cHorizontalLine horizontalLine;
    cButton buttonOk;
  };

  void cAboutDialog::OnInit()
  {
    SetClientSize(500, 500);

    cGDIPlusBitmap original;
    original.LoadFromFile(TEXT("C:\\Users\\media\\dev\\diesel\\data\\icons\\application_512x512.png"));

    cGDIPlusBitmap stretched;
    stretched.StretchFrom(original, 500, 500);

    cBitmap bitmap;
    stretched.CopyTo(bitmap);

    imageControl.Create(*this, bitmap);

    // Build a string of all the text on the dialog
    ostringstream_t o;
    o<<TEXT(BUILD_APPLICATION_NAME) TEXT("\n");
    o<<TEXT("Version ") TEXT(BUILD_APPLICATION_VERSION_STRING) TEXT("\n");

    spitfire::util::cDateTime dateTime;
    o<<TEXT(BUILD_APPLICATION_COPYRIGHT_OWNER)<<TEXT(", ")<<dateTime.GetYear()<<TEXT("\n");

    o<<TEXT("<a href=\"") TEXT(BUILD_APPLICATION_WEBSITE) TEXT("\">") TEXT(BUILD_APPLICATION_WEBSITE) TEXT("</a>");

    linkControl.Create(*this, *this, ID_ABOUTDIALOG_LINKCONTROL, o.str());

    // Add horizontal line
    horizontalLine.Create(*this);

    // Add standard buttons
    buttonOk.CreateOk(*this, TEXT("Ok"));
    SetFocus(buttonOk.GetHandle());

    LayoutAndSetWindowSize();

    // Make sure our controls start in the correct state
    EnableControls();
  }

  void cAboutDialog::OnLinkClicked(const cLinkControl& linkControl, size_t index)
  {
    (void)linkControl;

    if (index == 0) OpenWebPage(*this, TEXT(BUILD_APPLICATION_WEBSITE));
  }

  void cAboutDialog::LayoutAndSetWindowSize()
  {
    int iX = GetMarginWidth();
    int iY = GetMarginHeight();

    const int iWidth = CharacterUnitsToPixelsX(22) - (2 * GetSpacerWidth());

    const int iImageWidth = iWidth;
    const int iImageHeight = iWidth;

    cGDIPlusBitmap original;
    original.LoadFromFile(TEXT("C:\\Users\\media\\dev\\diesel\\data\\icons\\application_512x512.png"));

    cGDIPlusBitmap stretched;
    stretched.StretchFrom(original, iImageWidth, iImageHeight);

    cBitmap bitmap;
    stretched.CopyTo(bitmap);

    imageControl.SetImage(bitmap);
    
    const int iImageX = (iWidth - iImageWidth) / 2;
    MoveControl(imageControl.GetHandle(), iImageX, 0, iImageWidth, iImageHeight);
    iY += iImageHeight + GetSpacerHeight();

    const int iLinkControlHeight = MeasureStaticTextHeight(linkControl.GetHandle(), iWidth);
    MoveControl(linkControl.GetHandle(), iX, iY, iWidth, iLinkControlHeight);
    iY += iLinkControlHeight + GetSpacerHeight();

    MoveControl(horizontalLine.GetHandle(), iX, iY, iWidth, 1);
    iY += 1 + GetSpacerHeight();

    const int iOkButtonWidth = MeasureOkButtonWidth();
    const int iOkButtonX = (iWidth - iOkButtonWidth) / 2;
    MoveControl(buttonOk.GetHandle(), iOkButtonX, iY, iOkButtonWidth, GetButtonHeight());
    iY += GetButtonHeight();

    // Set our window size now that we know how big it should be
    const int iDialogWidth = GetMarginWidth() + iWidth + GetMarginWidth();
    const int iDialogHeight = iY + GetMarginHeight();
    SetClientSize(iDialogWidth, iDialogHeight);
  }

  bool cAboutDialog::OnOk()
  {
    /*int iCacheMaximumSizeGB = historyCacheMaximumSizeGB.GetValue();
    if ((iCacheMaximumSizeGB < 1) || (iCacheMaximumSizeGB > 20)) return false;

    About.SetMaximumCacheSizeGB(iCacheMaximumSizeGB);*/

    return true;
  }

  void cAboutDialog::EnableControls()
  {
  }

  bool cAboutDialog::Run(cWindow& parent)
  {
    return RunNonResizable(parent, TEXT("About"));
  }


  // ** OpenAboutDialog

  bool OpenAboutDialog(cWindow& parent)
  {
    cAboutDialog dialog;
    return dialog.Run(parent);
  }
}
