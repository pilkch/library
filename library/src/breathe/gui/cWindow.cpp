#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>

// Boost includes
#include <boost/shared_ptr.hpp>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

// OpenGL Headers
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe
#include <breathe/breathe.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/storage/filesystem.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindow.h>

namespace breathe
{
  namespace gui
  {
    const float WINDOW_TITLE_BAR_HEIGHT_ABSOLUTE = 0.025f;

    // This is just a quick test to make sure that a derived window compiles correctly at all times
    class cDerivedWindow : public cWindow
    {
    public:

    private:
      void _OnEvent(const cEvent& event);
    };

    void cDerivedWindow::_OnEvent(const cEvent& event)
    {
      switch (event.GetType()) {
        case EVENT_MOUSE_DOWN: {
          cWidget& widget = event.GetWidget();
          size_t button = event.GetMouseButton();
          const math::cVec2& position = event.GetMousePosition();
          //DoStuff();
          break;
        }
        case EVENT_MOUSE_UP: {
          cWidget& widget = event.GetWidget();
          size_t button = event.GetMouseButton();
          const math::cVec2& position = event.GetMousePosition();
          //DoStuff();
          break;
        }
        case EVENT_CONTROL_CHANGE_VALUE: {
          size_t button = event.GetMouseButton();
          if (button == MOUSE_BUTTON_PRIMARY) {
            cWidget& widget = event.GetWidget();
            switch (widget.GetID()) {
              case 100: {
                //CmOk();
                break;
              }
            }
          }
          break;
        }
      };
    }

    // *** cWindow

    cWindow::cWindow(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* _pParent, bool _bModeless, bool _bResizable) :
      cWidget(id, WIDGET_WINDOW, x, y, width, height),
      z(0),
      pChildContainer(nullptr)
    {
      SetCaption(caption);
      SetParent(_pParent);

      if (pParent != nullptr) z = ((cWindow*)pParent)->GetZDepth() + 1;

      bModeless = _bModeless;
      bResizable = _bResizable;

      pChildContainer = new cWidget_InvisibleContainer(GenerateID(), 0.0f, GetTitleBarHeightAbsolute(), 1.0f, 1.0f - GetTitleBarHeightAbsolute());
      AddChild(pChildContainer);
    }

    bool cWindow::AddChildToContainer(cWidget* pChild)
    {
      ASSERT(pChildContainer != nullptr);
      return pChildContainer->AddChild(pChild);
    }

    float cWindow::GetTitleBarHeightAbsolute() const
    {
      return WINDOW_TITLE_BAR_HEIGHT_ABSOLUTE;
    }

    void cWindow::Update(sampletime_t currentTime)
    {
    }

    void cWindow::OnMouseEvent(int button, int state, float x, float y)
    {
      LOG<<"cWindow::OnMouseEvent "<<x<<", "<<y<<std::endl;

      if (button == SDL_BUTTON_LEFT) {
        LOG<<"cWindow::OnMouseEvent SDL_BUTTON_LEFT"<<std::endl;

        cWidget* p = FindChildAtPoint(x, y);
        if (p != nullptr) LOG<<"cWindow::OnMouseEvent "<<breathe::string::ToUTF8(p->GetText())<<" visible="<<p->IsVisible()<<" enabled="<<p->IsEnabled()<<std::endl;

        while ((p != nullptr) && (!p->IsVisible() || !p->IsEnabled())) {
          p = p->GetParent();
          if (p != nullptr) LOG<<"cWindow::OnMouseEvent "<<breathe::string::ToUTF8(p->GetText())<<" visible="<<p->IsVisible()<<" enabled="<<p->IsEnabled()<<std::endl;
        }

        if (p != nullptr) LOG<<"cWindow::OnMouseEvent "<<breathe::string::ToUTF8(p->GetText())<<" visible="<<p->IsVisible()<<" enabled="<<p->IsEnabled()<<std::endl;

        if (p != nullptr) {
          if (state != 0) p->OnLeftMouseDown(x, y);
          else p->OnLeftMouseUp(x, y);
          LOG<<"cWindow::OnMouseEvent "<<breathe::string::ToUTF8(p->GetText())<<std::endl;
        } else LOG<<"cWindow::OnMouseEvent "<<breathe::string::ToUTF8(GetText())<<std::endl;
      }

      //_OnEvent(button, state, x, y);
    }


    // *** cModelessWindow

    cModelessWindow::cModelessWindow(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* pParent) :
      cWindow(id, x, y, width, height, caption, pParent, true, true)
    {
    }


    // *** cModalDialog

    cModalDialog::cModalDialog(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* pParent) :
      cWindow(id, x, y, width, height, caption, pParent, false, false)
    {
    }
  }
}
