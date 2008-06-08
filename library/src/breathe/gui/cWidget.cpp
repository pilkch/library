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

// OpenGL Headers
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

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
		id_t nextID = 1;

		id_t GenerateID()
		{
			return nextID++;
		}

		cWidget::cWidget(id_t _idControl, WIDGET_TYPE _type, float _x, float _y, float _width, float _height) :
			pParent(NULL),
			idControl(_idControl),
			type(_type),

			x(_x),
			y(_y),
			width(_width),
			height(_height),

			bEnabled(true),
			bVisible(true),
			bResizable(false),

			minimum(0),
			maximum(100),
			value(0)
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

    cWidget* cWidget::FindChild(id_t _idControl) const
		{
			if (_idControl == idControl) return const_cast<cWidget*>(this);

			size_t n = child.size();
			cWidget* p = nullptr;
			for (size_t i = 0; i < n; i++) {
				p = child[i]->FindChild(_idControl);
				if (p != nullptr) return p;
			}

			return nullptr;
		}

    cWidget* cWidget::FindChildAtPoint(float _x, float _y) const
    {
			size_t n = child.size();
			cWidget* p = nullptr;
			for (size_t i = 0; i < n; i++) {
        p = child[i];
        if (math::PointIsWithinBounds(_x, _y,
          p->GetX(), p->GetY(), p->GetWidth(), p->GetHeight())) return p->FindChildAtPoint(_x, _y);
			}

      return const_cast<cWidget*>(this);
    }

    void cWidget::SendCommandToParentWindow(id_t uiCommand)
    {
      cWidget* pParent = GetParent();
      while (pParent != nullptr) {
        if (pParent->IsAWindow()) {
          cWindow* pParentWindow = (cWindow*)pParent;
          pParentWindow->OnEvent(uiCommand);
          return;
        }

        pParent = pParent->GetParent();
      }

      printf("cWidget::SendCommandToParentWindow FAILED Parent window not found\n");
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

		float cWidget::HorizontalRelativeToAbsolute(float n) const
		{
			if (pParent != nullptr) return pParent->GetX() + (n * pParent->HorizontalRelativeToAbsolute(pParent->GetWidth()));
			return n;
		}

		float cWidget::VerticalRelativeToAbsolute(float n) const
		{
			if (pParent != nullptr) return pParent->GetY() + (n * pParent->VerticalRelativeToAbsolute(pParent->GetHeight()));
			return n;
		}

		void cWidget::SetPosition(float _x, float _y)
		{

		}

    bool cWidget::GetEventHandler(event_t event, id_t& outID) const
    {
      std::map<event_t, id_t>::const_iterator iter = handlers.begin();
      std::map<event_t, id_t>::const_iterator iterEnd = handlers.end();
      while (iter != iterEnd) {
        if (iter->first == event) {
          outID = iter->second;
          return true;
        }
      }

      return false;
    }

    void cWidget::CheckAndHandleEvent(event_t event)
    {
      id_t idOut;
      if (!GetEventHandler(event, idOut)) return;

      printf("Found event handler %d\n", idOut);
      SendCommandToParentWindow(idOut);
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
      if (bCurrentlyClickingOnThisControl) {
        printf("Clicking on control %d\n", idControl);
        CheckAndHandleEvent(EVENT_CLICK_PRIMARY);
      }

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
