#include <cassert>

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

#include <SDL/SDL_image.h>

// Breathe
#include <breathe/breathe.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>

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
#include <breathe/gui/cWindowManager.h>

const float fTextureWidth = 64.0f;
const float fOneOverTextureWidth = 1.0f / fTextureWidth;

float CreateTextureCoord(float value) { return value * fOneOverTextureWidth; }

namespace breathe
{
  namespace gui
  {
    render::material::cMaterialRef pMaterial;
    std::vector<render::cTextureRef> textureBackground;
    render::cFont* pFontWindowCaption = nullptr;

    enum
    {
      BACKGROUND_NORMAL,
      BACKGROUND_DARKER,
      BACKGROUND_DARKEST,
      BACKGROUND_TEXT
    };

    cWindowManager::cWindowManager() :
      pEventWidgetMouseLeftButtonDown(nullptr)
    {
    }

    cWindowManager::~cWindowManager()
    {
      child_iterator iter = child.begin();
      child_iterator iterEnd = child.end();
      while (iter != iterEnd) {
        SAFE_DELETE(*iter);

        iter++;
      }
      child.clear();

      SAFE_DELETE(pFontWindowCaption);
    }

    void cWindowManager::LoadTheme()
    {
      pMaterial = pRender->AddMaterial("gui.mat");

      textureBackground.push_back(pRender->AddTexture("gui_background_normal.png"));
      textureBackground.push_back(pRender->AddTexture("gui_background_darker.png"));
      textureBackground.push_back(pRender->AddTexture("gui_background_darkest.png"));
      textureBackground.push_back(pRender->AddTexture("gui_background_text.png"));

      ASSERT(pMaterial != nullptr);
      ASSERT(textureBackground[0] != nullptr);
      ASSERT(textureBackground[1] != nullptr);
      ASSERT(textureBackground[2] != nullptr);
      ASSERT(textureBackground[3] != nullptr);

      //pFontWindowCaption = new render::cFont(TEXT("osx_fonts/Lucida Grande.ttf"), 10);
      breathe::string_t sFilename;
      breathe::filesystem::FindResourceFile("fonts/pricedown.ttf", sFilename);
      pFontWindowCaption = new breathe::render::cFont(sFilename, 32);
    }

    void cWindowManager::LoadGuiFromXML(const string_t& sFilename)
    {
      breathe::xml::cNode root(sFilename);
      breathe::xml::cNode::iterator iter(root);

      if (!iter.IsValid()) return;

      iter.FindChild("gui");
      if (!iter.IsValid()) return;

      string_t sWindowType;
      cWindow* pWindow = nullptr;
      while (iter.IsValid()) {
        pWindow = nullptr;
        sWindowType = iter.GetName();

        string_t sID;
        id_t idWindow = 0;
        if (iter.GetAttribute(TEXT("id"), sID)) idWindow = GetIDFromStringIdentifier(sID);
        else idWindow = GenerateID();

        if (sWindowType == TEXT("modelesswindow")) {
//          pWindow = new cModelessWindow(idWindow, x, y, width, height, caption, pParent);
        }

        iter++;
      };
    }

    id_t cWindowManager::GetIDFromStringIdentifier(const string_t& sIdentifier)
    {
      std::map<string_t, id_t>::iterator iter = stringIdentifierToID.begin();
      std::map<string_t, id_t>::iterator iterEnd = stringIdentifierToID.end();
      while (iter != iterEnd) {
        if (iter->first == sIdentifier) return iter->second;
        iter++;
      }

      return GenerateID();
    }

    cWindow* cWindowManager::_FindWindowUnderPoint(float x, float y) const
    {
      cWindow* pWindow = nullptr;
      child_const_iterator iter = child.begin();
      child_const_iterator iterEnd = child.end();
      cWindow* pTempWindow = nullptr;
      while (iter != iterEnd) {
        pTempWindow = (*iter);
        if (math::PointIsWithinBounds(x, y,
          pTempWindow->GetX(), pTempWindow->GetY(), pTempWindow->GetWidth(), pTempWindow->GetHeight())) {
          if (pWindow == nullptr) pWindow = pTempWindow;
          else if (pTempWindow->GetZDepth() > pWindow->GetZDepth()) pWindow = pTempWindow;
        }
        iter++;
      }

      return pWindow;
    }

    bool cWindowManager::OnMouseEvent(int button, int state, float x, float y)
    {
      cWindow* pWindow = _FindWindowUnderPoint(x, y);
      if (pWindow != nullptr) {
        float fScaleX = 1.0f / pWindow->GetWidth();
        float fScaleY = 1.0f / pWindow->GetHeight();
        float x2 = x * fScaleX;
        float y2 = y * fScaleY;
        pWindow->OnMouseEvent(button, state, x2, y2);
        return true;
      }

      /*if (button is down) {
        cEvent event(EVENT_MOUSE_DOWN);

        event.SetWidget(this);
        event.SetMouseButton(button);
        event.SetMousePosition(position);

        pWindow->OnEvent(event);
      } else if (button is up) {
        cEvent event(EVENT_MOUSE_UP);

        event.SetWidget(this);
        event.SetMouseButton(button);
        event.SetMousePosition(position);

        pWindow->OnEvent(event);
      }*/

      return false;
    }

    void cWindowManager::Update(sampletime_t currentTime)
    {
      //if ()
    }

    bool cWindowManager::AddChild(cWindow* pChild)
    {
      ASSERT(pChild != nullptr);
      child.push_back(pChild);
      pChild->pParent = nullptr;

      return true;
    }

    bool cWindowManager::RemoveChild(cWindow* pChild)
    {
      ASSERT(pChild != nullptr);
      child.remove(pChild);

      // Ok, now that it has been removed from the window manager it does not exist any more so we delete it.
      // This operation is recursive, every child of pChild will also be deleted.  Any external references
      // will be invalid after this.
      SAFE_DELETE(pChild);

      return true;
    }

    // This is just for the first version to get something rendered, don't worry about texturing, just draw
    // a plain solid colour filled rectangle with a border
    void cWindowManager::_RenderRectangle(float fX, float fY, float fWidth, float fHeight) const
    {
      static const math::cColour box(0.4f, 0.4f, 0.4f);
      static const math::cColour upperBorder(0.6f, 0.6f, 0.6f);
      static const math::cColour lowerBorder(0.3f, 0.3f, 0.3f);

      pRender->RenderScreenSpaceSolidRectangleWithBorderTopLeftIsAt(fX, fY, fWidth, fHeight, box, upperBorder, lowerBorder);
    }

    void cWindowManager::_RenderWindow(const cWindow& widget) const
    {
      const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
      const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

      const float absolute_bar_height = widget.VerticalRelativeToAbsolute(widget.GetTitleBarHeightAbsolute());

      // Draw the top bar
      _RenderRectangle(widget.GetXAbsolute(), widget.GetYAbsolute(), absolute_width, absolute_bar_height);

      // Draw the rest of the window
      _RenderRectangle(widget.GetXAbsolute(), widget.GetYAbsolute() + absolute_bar_height, absolute_width, absolute_height - absolute_bar_height);

      /*

      const float bar_v = 0.04f;

      // Draw the top left corner
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.GetXAbsolute(), widget.GetYAbsolute(), 0.02f, bar_height,
        0.0f, 0.0f, 0.05f, bar_v);

      // Draw the top right corner
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.GetXAbsolute() + absolute_width - 0.016f, widget.GetYAbsolute(), 0.016f, bar_height,
        0.187f, 0.0f, 0.049f, bar_v);

      // Draw the top bar
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.GetXAbsolute() + 0.02f, widget.GetYAbsolute(), absolute_width - 0.02f - 0.01f, bar_height,
        0.18f, 0.0f, 0.01f, bar_v);

      // TODO: Draw the caption

      // TODO: Draw the close button

      // TODO: Draw the minimise and maximise buttons

      if (widget.IsResizable()) {
        // TODO: Draw the bottom right corner
        //pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        //  widget.GetXAbsolute() + 0.02f, widget.GetYAbsolute(), absolute_width - 0.02f - 0.01f, 0.05f,
        //  0.18f, 0.0f, 0.01f, 0.1f);
      }

      // Draw the window background
      float width = absolute_width;
      float height = absolute_height - bar_height;
      render::ApplyTexture apply(textureBackground[BACKGROUND_NORMAL]);
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.GetXAbsolute(), widget.GetYAbsolute() + bar_height, absolute_width, absolute_height - bar_height,
        0.0f, 0.0f, CreateTextureCoord(width), CreateTextureCoord(height));*/

      // Draw the caption if this window has one
      if (widget.HasCaption()) {
        glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
          pRender->ClearMaterial();

          pRender->SelectTextureUnit0();

          glMatrixMode(GL_TEXTURE);
          glPushMatrix();
            glLoadIdentity();

            glMatrixMode(GL_MODELVIEW);

            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

            //pFontWindowCaption->printfCenteredHorizontallyVertically(widget.GetXAbsolute(), widget.GetYAbsolute(),
            //  widget.GetWidth(), bar_height,
            //  breathe::string::ToUTF8(widget.GetCaption()).c_str());
            pFontWindowCaption->PrintCenteredHorizontally(widget.GetXAbsolute(), widget.GetYAbsolute(),
              widget.GetWidth(), widget.GetCaption());

            glMatrixMode(GL_TEXTURE);
          glPopMatrix();

          pRender->SetMaterial(pMaterial);
        glPopAttrib();

        pRender->ClearMaterial();
      }
    }

    void cWindowManager::_RenderInput(const cWidget_Input& widget) const
    {
      const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
      const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

      _RenderRectangle(widget.GetXAbsolute(), widget.GetYAbsolute(), absolute_width, absolute_height);

      /*render::ApplyTexture apply(textureBackground[BACKGROUND_TEXT]);
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.HorizontalRelativeToAbsolute(widget.GetXAbsolute()), widget.VerticalRelativeToAbsolute(widget.GetYAbsolute()),
        absolute_width, absolute_height,
        0.0f, 0.0f, CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));*/
    }

    void cWindowManager::_RenderButton(const cWidget_Button& widget) const
    {
      const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
      const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

      _RenderRectangle(widget.GetXAbsolute(), widget.GetYAbsolute(), absolute_width, absolute_height);

      /*pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.HorizontalRelativeToAbsolute(widget.GetXAbsolute()), widget.VerticalRelativeToAbsolute(widget.GetYAbsolute()),
        absolute_width, absolute_height,
        0.0083f, 0.073f, 0.08f, 0.045f);//CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));
      */

      // Draw the text of this widget
      glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
        pRender->ClearMaterial();

        pRender->SelectTextureUnit0();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
          glLoadIdentity();

          glMatrixMode(GL_MODELVIEW);

          glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

          pFontWindowCaption->PrintCenteredHorizontallyVertically(
            widget.GetXAbsolute(), widget.GetYAbsolute(),
            absolute_width, absolute_height,
            widget.GetText()
          );

          glMatrixMode(GL_TEXTURE);
        glPopMatrix();

        pRender->SetMaterial(pMaterial);
      glPopAttrib();

      pRender->ClearMaterial();
    }

    void cWindowManager::_RenderStaticText(const cWidget_StaticText& widget) const
    {
      const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
      const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

      _RenderRectangle(widget.GetXAbsolute(), widget.GetYAbsolute(), absolute_width, absolute_height);

      /*render::ApplyTexture apply(textureBackground[BACKGROUND_TEXT]);
      pRender->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.HorizontalRelativeToAbsolute(widget.GetXAbsolute()), widget.VerticalRelativeToAbsolute(widget.GetYAbsolute()),
        absolute_width, absolute_height,
        0.0f, 0.0f, CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));
      */

      render::cFont* pFont = widget.GetFont();
      if (pFont == nullptr) pFont = pFontWindowCaption;

      pRender->BeginRenderingText();
        breathe::constant_stack<std::string>::reverse_iterator iter = CONSOLE.rbegin();
        breathe::constant_stack<std::string>::reverse_iterator iterEnd = CONSOLE.rend();
        unsigned int y = 60;
        while(iter != iterEnd) {
          pRender->SetColour(widget.GetColour());
          pFont->printf(0, static_cast<float>(y), (*iter).c_str());
          y += 30;

          iter++;
        };
      pRender->EndRenderingText();

      pRender->ClearMaterial();
    }

    void cWindowManager::_RenderWidget(const cWidget& widget) const
    {
      switch(widget.GetType())
      {
        case WIDGET_WINDOW:
          _RenderWindow(static_cast<const cWindow&>(widget));
          break;

        case WIDGET_BUTTON:
          _RenderButton(static_cast<const cWidget_Button&>(widget));
          break;

        case WIDGET_INPUT:
          //_RenderInput(widget);
          break;

        case WIDGET_STATICTEXT:
          _RenderStaticText(static_cast<const cWidget_StaticText&>(widget));
          break;

        default:
          ;
          //pRender->RenderScreenSpaceRectangleTopLeftIsAt(widget.GetXAbsolute(), widget.GetYAbsolute(), widget.GetWidth(), widget.GetHeight());
      };
    }

    void cWindowManager::_RenderChildren(const cWidget& widget) const
    {
      if (false == widget.IsVisible()) return;

      _RenderWidget(widget);

      const size_t n = widget.child.size();
      if (n == 0) return;

      for (size_t i = 0; i < n; i++) _RenderChildren(*widget.child[i]);
    }

    void cWindowManager::Render() const
    {
      ASSERT(pMaterial != nullptr);

      pRender->ClearMaterial();

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();

        child_const_iterator iter = child.begin();
        child_const_iterator iterEnd = child.end();
        while (iter != iterEnd) {
          _RenderChildren(*(*iter));

          iter++;
        }

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }
  }
}
