#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <iostream>
#include <fstream>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>

// Boost headers
#include <boost/shared_ptr.hpp>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// OpenGL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindow.h>
#include <breathe/gui/cMessageBox.h>

namespace breathe
{
  namespace gui
  {
    class cMessageBoxWindow : public cModalDialog
    {
    public:
      cMessageBoxWindow(const cMessageBoxSettings& settings, cMessageBoxListener& listener);

      void OnEvent(const cEvent& event);

    private:
      cWidget* pYesButton;
      cWidget* pNoButton;
      cWidget* pCancelButton;

      const cMessageBoxSettings& settings;
      cMessageBoxListener& listener;
    };

    cMessageBoxWindow::cMessageBoxWindow(const cMessageBoxSettings& _settings, cMessageBoxListener& _listener) :
      settings(_settings),
      listener(_listener)
    {
    }

    void Open()
    {
      SetTitle(settings.GetTitle());

      // Every message box is modal for now
      //settings.IsModeless();

      // Icon
      {
        cWidget* pWidget = new cImage;

        cImage::STANDARD_IMAGE image = cImage::STANDARD_INFORMATION;

        const cMessageBoxSettings::TYPE type = settings.GetType();
        if (type == TYPE_INFORMATION) image = cImage::STANDARD_INFORMATION;
        else if (type == TYPE_WARNING) image = cImage::STANDARD_WARNING;
        else image = cImage::STANDARD_ERROR;

        pWidget->SetImage(image);

        AddWidget(pWidget);
      }

      // Static text
      {
        cWidget* pWidget = new cStaticText;
        pWidget->SetText(settings.GetText());
        AddWidget(pWidget);
      }

      // Yes button
      {
        pYesButton = new cDefaultButton;
        pYesButton->SetText(settings.GetYesButtonText());
        AddWidget(pYesButton);
      }

      // No button
      if (settings.HasNoButton()) {
        ASSERT(pNoButton == nullptr);
        pNoButton = new cButton;
        pNoButton->SetText(settings.GetNoButtonText());

        AddWidget(pNoButton);
      }

      // Cancel button
      if (settings.HasCancelButton()) {
        pCancelButton = new cCancelButton;
        AddWidget(pCancelButton);
      }
    }

    void cMessageBoxWindow::OnEvent(const cEvent& event)
    {
      if (event.type == cEvent::TYPE_CLICK) {
        switch (event.widget) {
          case pYesButton: {
            listener.OnMessageBoxReturnYes();
            PopState();
            break;
          }
          case pNoButton: {
            listener.OnMessageBoxReturnNo();
            PopState();
            break;
          }
          case pCancelButton: {
            listener.OnMessageBoxReturnCancel();
            PopState();
            break;
          }
        };
      }
    }

    class cMessageBoxState : public cApplication::cAppState
    {
    public:
      cMessageBoxState(const cMessageBoxSettings& settings, cMessageBoxListener& listener);

    private:
      const cMessageBoxSettings& settings;
      cMessageBoxListener& listener;
    };

    cMessageBoxState::cMessageBoxState(const cMessageBoxSettings& _settings, cMessageBoxListener& _listener) :
      settings(_settings),
      listener(_listener),
      pNoButton(nullptr)
    {
    }

    void cMessageBoxState::Open()
    {
      cMessageBoxWindow* pWindow = new cMessageBoxWindow(settings, listener);
      add_window_to_window_manager(pWindow);
    }

    void MESSAGE_BOX::OpenMessageBox(const cMessageBoxSettings& settings, cMessageBoxListener& listener)
    {
      cMessageBoxState* pState = new state(settings, listener);
      PushState(pState);
    }

    // For any dialog or message box
    // The default button is required and its default text is L__Yes, this is changeable
    // The cancel button is optional and its text is always L__Cancel, this is not changeable
    // The no button appears on a messagebox, is optional and its default text is L__No, this is changeable
  }
}
