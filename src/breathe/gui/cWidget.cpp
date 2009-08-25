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

// SDL headers
#include <SDL/SDL.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/cSmartPtr.h>
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

namespace breathe
{
  namespace gui
  {
    id_t nextID = 1;

    id_t GenerateID()
    {
      return nextID++;
    }


    // cWidget

    cWidget::cWidget(id_t _idControl, WIDGET_TYPE _type, float _x, float _y, float _width, float _height) :
      pParent(NULL),
      idControl(_idControl),
      type(_type),

      minimum(0),
      maximum(100),
      value(0),

      x(_x),
      y(_y),
      width(_width),
      height(_height),

      bEnabled(true),
      bVisible(true),
      bResizable(false)
    {

    }

    cWidget::~cWidget()
    {
      size_t n = child.size();
      for (size_t i = 0; i < n; i++)
        SAFE_DELETE(child[i]);
    }

    bool cWidget::AddChild(cWidget* pChild)
    {
      child.push_back(pChild);
      pChild->pParent = this;

      return true;
    }

    cWidget* cWidget::FindChild(id_t _idControl)
    {
      if (_idControl == idControl) return this;

      cWidget* p = nullptr;

      const size_t n = child.size();
      for (size_t i = 0; i < n; i++) {
        p = child[i]->FindChild(_idControl);
        if (p != nullptr) return p;
      }

      return nullptr;
    }

    cWidget* cWidget::FindChildAtPoint(float absoluteX, float absoluteY)
    {
      LOG<<"cWidget::FindChildAtPoint "<<absoluteX<<", "<<absoluteY<<", ("<<GetXAbsolute()<<", "<<GetYAbsolute()<<"), ("<<GetWidthAbsolute()<<", "<<GetHeightAbsolute()<<")"<<std::endl;

      cWidget* p = nullptr;

      // Let's see if we can find a widget that is higher up and still contains the coordinate
      const size_t n = child.size();
      for (size_t i = 0; i < n; i++) {
        p = child[i];
        if (math::PointIsWithinBounds(absoluteX, absoluteY,
          p->GetXAbsolute(), p->GetYAbsolute(), p->GetWidthAbsolute(), p->GetHeightAbsolute())) return p->FindChildAtPoint(absoluteX, absoluteY);
      }

      return this;
    }

    void cWidget::SendEventToParentWindow(const cEvent& event)
    {
      cWidget* pParent = GetParent();
      while (pParent != nullptr) {
        if (pParent->IsAWindow()) {
          cWindow* pParentWindow = (cWindow*)pParent;
          LOG<<"cWidget::SendEventToParentWindow Sending event to parent window"<<std::endl;
          pParentWindow->OnEvent(event);
          return;
        }

        pParent = pParent->GetParent();
      }

      printf("cWidget::SendEventToParentWindow FAILED Parent window not found\n");
    }

    bool cWidget::IsEnabled() const
    {
      if (pParent != nullptr) return bEnabled && pParent->IsEnabled();
      return bEnabled;
    }

    bool cWidget::IsVisible() const
    {
      if (pParent != nullptr) return bVisible && pParent->IsVisible();
      return bVisible;
    }

    float cWidget::GetXAbsolute() const
    {
      if (pParent != nullptr) return (pParent->GetXAbsolute() + HorizontalRelativeToAbsolute(x));
      return HorizontalRelativeToAbsolute(x);
    }

    float cWidget::GetYAbsolute() const
    {
      if (pParent != nullptr) return (pParent->GetYAbsolute() + VerticalRelativeToAbsolute(y));
      return VerticalRelativeToAbsolute(y);
    }

    float cWidget::GetWidthAbsolute() const
    {
      return VerticalRelativeToAbsolute(width);
    }

    float cWidget::GetHeightAbsolute() const
    {
      return VerticalRelativeToAbsolute(height);
    }

    float cWidget::HorizontalRelativeToAbsolute(float n) const
    {
      if (pParent != nullptr) return (n * pParent->HorizontalRelativeToAbsolute(pParent->GetWidth()));
      return n;
    }

    float cWidget::VerticalRelativeToAbsolute(float n) const
    {
      if (pParent != nullptr) return (n * pParent->VerticalRelativeToAbsolute(pParent->GetHeight()));
      return n;
    }

    void cWidget::SetPosition(float _x, float _y)
    {

    }

    bool cWidget::GetEventHandler(EVENT event, id_t& outID) const
    {
      std::map<EVENT, id_t>::const_iterator iter = handlers.begin();
      std::map<EVENT, id_t>::const_iterator iterEnd = handlers.end();
      while (iter != iterEnd) {
        if (iter->first == event) {
          outID = iter->second;
          return true;
        }
      }

      return false;
    }

    void cWidget::CheckAndHandleEvent(EVENT event)
    {
      LOG<<"cWidget::CheckAndHandleEvent"<<std::endl;

      //id_t idOut;
      //if (!GetEventHandler(event, idOut)) return;

      //LOG<<"Found event handler "<<idOut<<std::endl;
      //SendCommandToParentWindow(idOut);


      cEvent e(event);
      e.SetWidget(this);
      e.SetMouseButton(MOUSE_BUTTON_PRIMARY); // TODO: This is incorrect, it should actually accept this value as a parameter
      const math::cVec2 point(0.0f, 0.0f); // TODO: This is incorrect, it should actually accept this value as a parameter
      e.SetMousePosition(point);
      SendEventToParentWindow(e);
    }

    /*
    cWidget_Input

    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    std::string currentline(CONSOLE.GetCurrentLine());
    if (CONSOLE.uiCursorBlink>20) pFont->printf(0, 20, currentline.c_str());
    else
      pFont->printf(0, 20, (
        currentline.substr(0, CONSOLE.uiCursorPosition) + "|" +
        currentline.substr(CONSOLE.uiCursorPosition)).c_str());
    */

    // ** cWidget_Button
    void cWidget_Button::_OnLeftMouseUp(float x, float y)
    {
      //if (bCurrentlyClickingOnThisControl) {
        LOG<<"cWidget_Button::_OnLeftMouseUp Clicking on control "<<idControl<<std::endl;
        CheckAndHandleEvent(EVENT_MOUSE_UP);
      //}

      bCurrentlyClickingOnThisControl = false;
    }


    // ** cWidget_SliderHorizontal
    void cWidget_SliderHorizontal::_OnLeftMouseUp(float x, float y)
    {
      value = int(100.0f * fTempValue);
    }

    void cWidget_SliderHorizontal::_OnMouseMove(float x, float y)
    {
      LOG<<"cWidget_SliderHorizontal::_OnMouseMove"<<std::endl;
      fTempValue = x;
    }
  }
}
