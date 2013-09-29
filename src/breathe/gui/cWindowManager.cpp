#include <cassert>

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

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

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

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cResourceManager.h>
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
    const float fAlpha = 1.0f;

    class cTheme
    {
    public:
      cTheme();

      const math::cColour& GetColourWindowBackground() const { return colourWindowBackground; }
      const math::cColour& GetColourWindowTitleBar() const { return colourWindowTitleBar; }
      const math::cColour& GetColourControlPrimary() const { return colourControlPrimary; }
      const math::cColour& GetColourControlSecondary() const { return colourControlSecondary; }

    private:
      math::cColour colourWindowBackground;
      math::cColour colourWindowTitleBar;
      math::cColour colourControlPrimary;
      math::cColour colourControlSecondary;
    };

    cTheme::cTheme() :
      colourWindowBackground(0.4f, 0.4f, 0.4f),
      colourWindowTitleBar(0.6f, 0.6f, 0.6f),
      colourControlPrimary(0.6f, 0.6f, 0.6f),
      colourControlSecondary(0.3f, 0.3f, 0.3f)
    {
    }

    cTheme theme;


    render::material::cMaterialRef pMaterial;
    render::cFont* pFontWindowCaption = nullptr;


    cWindowManager::cWindowManager() :
      pEventWidgetMouseLeftButtonDown(nullptr)
    {
    }

    cWindowManager::~cWindowManager()
    {
      child_iterator iter = child.begin();
      const child_iterator iterEnd = child.end();
      while (iter != iterEnd) {
        SAFE_DELETE(*iter);

        iter++;
      }
      child.clear();

      SAFE_DELETE(pFontWindowCaption);
    }

    void cWindowManager::LoadTheme()
    {
      pMaterial = pResourceManager->AddMaterial(TEXT("materials/gui.mat"));
      ASSERT(pMaterial != nullptr);

      //pFontWindowCaption = new render::cFont(TEXT("osx_fonts/Lucida Grande.ttf"), 10);
      breathe::string_t sFilename;
      breathe::filesystem::FindResourceFile(TEXT("fonts/pricedown.ttf"), sFilename);
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
        sWindowType = breathe::string::ToString_t(iter.GetName());

        string_t sID;
        id_t idWindow = 0;
        if (iter.GetAttribute("id", sID)) {
          idWindow = GetIDFromStringIdentifier(sID);
        } else idWindow = GenerateID();

        if (sWindowType == TEXT("modelesswindow")) {
//          pWindow = new cModelessWindow(idWindow, x, y, width, height, caption, pParent);
        }

        iter++;
      };
    }

    id_t cWindowManager::GetIDFromStringIdentifier(const string_t& sIdentifier)
    {
      std::map<string_t, id_t>::iterator iter = stringIdentifierToID.begin();
      const std::map<string_t, id_t>::iterator iterEnd = stringIdentifierToID.end();
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
      const child_const_iterator iterEnd = child.end();
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
        pWindow->OnMouseEvent(button, state, x, y);
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

    void cWindowManager::Update(durationms_t currentTime)
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
      //pContext->RenderScreenSpaceSolidRectangleWithBorderTopLeftIsAt(fX, fY, fWidth, fHeight, box, upperBorder, lowerBorder);
      pContext->RenderScreenSpaceRectangleTopLeftIsAt(fX, fY, fWidth, fHeight);
    }

    void SetColourFromThemeColourAndAlpha(const math::cColour& themeColour)
    {
      pContext->SetColour(math::cColour(themeColour.r, themeColour.g, themeColour.b, fAlpha));
    }

    void cWindowManager::_RenderWindow(const cWindow& widget) const
    {
      const float title_bar_height = widget.GetTitleBarHeight();

      // Draw the top bar
      SetColourFromThemeColourAndAlpha(theme.GetColourWindowTitleBar());
      _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), title_bar_height);

      // Draw the rest of the window
      SetColourFromThemeColourAndAlpha(theme.GetColourWindowBackground());
      _RenderRectangle(widget.GetX(), widget.GetY() + title_bar_height, widget.GetWidth(), widget.GetHeight() - title_bar_height);

#if 0
      // Draw the caption if this window has one
      if (widget.HasCaption()) {
        /*glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
          {
            render::ApplyMaterial apply(pMaterial);

            pContext->SelectTextureUnit0();

            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
              glLoadIdentity();

              glMatrixMode(GL_MODELVIEW);

              glColor4f(0.0f, 0.0f, 0.0f, 1.0f);*/

              //pFontWindowCaption->printfCenteredHorizontallyVertically(widget.GetX(), widget.GetY(),
              //  widget.GetWidth(), bar_height,
              //  breathe::string::ToUTF8(widget.GetCaption()).c_str());
              pFontWindowCaption->PrintCenteredHorizontally(widget.GetX(), widget.GetY(),
                widget.GetWidth(), widget.GetCaption());

              //glMatrixMode(GL_TEXTURE);
            //glPopMatrix();
          //}
        //glPopAttrib();
      }
#endif
    }

    void cWindowManager::_RenderInput(const cWidget_Input& widget) const
    {
      SetColourFromThemeColourAndAlpha(theme.GetColourControlPrimary());
      _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());
    }

    void cWindowManager::_RenderButton(const cWidget_Button& widget) const
    {
      SetColourFromThemeColourAndAlpha(theme.GetColourControlPrimary());
      _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());

      /*pContext->RenderScreenSpaceRectangleTopLeftIsAt(
        widget.HorizontalRelativeToAbsolute(widget.GetXAbsolute()), widget.VerticalRelativeToAbsolute(widget.GetYAbsolute()),
        absolute_width, absolute_height,
        0.0083f, 0.073f, 0.08f, 0.045f);//CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));
      */

      /*// Draw the text of this widget
      glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
        pContext->UnApplyMaterial(pMaterial);

        pContext->SelectTextureUnit0();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
          glLoadIdentity();

          glMatrixMode(GL_MODELVIEW);

          glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

          pFontWindowCaption->PrintCenteredHorizontallyVertically(
            widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight(),
            widget.GetText()
          );

          glMatrixMode(GL_TEXTURE);
        glPopMatrix();

        pContext->ApplyMaterial(pMaterial);
      glPopAttrib();

      pContext->ClearMaterial();*/
    }

    void cWindowManager::_RenderStaticText(const cWidget_StaticText& widget) const
    {
      SetColourFromThemeColourAndAlpha(theme.GetColourControlPrimary());
      _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());

      /*render::cFont* pFont = widget.GetFont();
      if (pFont == nullptr) pFont = pFontWindowCaption;

      pContext->BeginRenderingText();
        breathe::constant_stack<std::string>::reverse_iterator iter = CONSOLE.rbegin();
        breathe::constant_stack<std::string>::reverse_iterator iterEnd = CONSOLE.rend();
        unsigned int y = 60;
        while(iter != iterEnd) {
          pContext->SetColour(widget.GetColour());
          pFont->printf(0, static_cast<float>(y), (*iter).c_str());
          y += 30;

          iter++;
        };
      pContext->EndRenderingText();

      pContext->ClearMaterial();*/
    }

    void cWindowManager::_RenderWidget(const cWidget& widget) const
    {
      std::cout<<"cWindowManager::_RenderWidget"<<std::endl;
      switch(widget.GetType()) {
        case WIDGET_TYPE::WINDOW:
          std::cout<<"cWindowManager::_RenderWidget WIDGET_TYPE::WINDOW"<<std::endl;
          _RenderWindow(static_cast<const cWindow&>(widget));
          break;

        case WIDGET_TYPE::BUTTON:
          std::cout<<"cWindowManager::_RenderWidget WIDGET_TYPE::WINDOW"<<std::endl;
          _RenderButton(static_cast<const cWidget_Button&>(widget));
          break;

        case WIDGET_TYPE::INPUT:
          std::cout<<"cWindowManager::_RenderWidget WIDGET_TYPE::INPUT"<<std::endl;
          _RenderInput(static_cast<const cWidget_Input&>(widget));
          break;

        case WIDGET_TYPE::STATICTEXT:
          std::cout<<"cWindowManager::_RenderWidget WIDGET_TYPE::STATICTEXT"<<std::endl;
          _RenderStaticText(static_cast<const cWidget_StaticText&>(widget));
          break;

        case WIDGET_TYPE::INVISIBLE_CONTAINER:
          std::cout<<"cWindowManager::_RenderWidget WIDGET_TYPE::INVISIBLE_CONTAINER"<<std::endl;
          // Purposely do not render this control
          SetColourFromThemeColourAndAlpha(math::cColour(1.0f, 0.0f, 0.0f, 1.0f));
          _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());
          break;

        default:
          std::cout<<"cWindowManager::_RenderWidget UNKNOWN WIDGET TYPE"<<std::endl;
          //SetColourFromThemeColourAndAlpha(theme.GetColourControlPrimary());
          SetColourFromThemeColourAndAlpha(math::cColour(math::randomZeroToOnef(), math::randomZeroToOnef(), math::randomZeroToOnef(), 1.0f));
          _RenderRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());
      };
    }

    void cWindowManager::_RenderChildren(const cWidget& widget) const
    {
      std::cout<<"cWindowManager::_RenderChildren "<<widget.GetX()<<", "<<widget.GetY()<<", "<<widget.GetWidth()<<"x"<<widget.GetHeight()<<std::endl;
      if (!widget.IsVisible()) return;

      _RenderWidget(widget);

      const size_t n = widget.child.size();
      if (n == 0) return;

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

        glTranslatef(widget.GetX(), widget.GetY(), 0.0f);

        float fScaleHorizontal = 1.0f;
        float fScaleVertical = 1.0f;

        cWidget* pParent = widget.GetParent();
        if (pParent != nullptr) {
          fScaleHorizontal = pParent->GetWidth();
          fScaleVertical = pParent->GetHeight();
        }

        glScalef(fScaleHorizontal, fScaleVertical, 1.0f);

        for (size_t i = 0; i < n; i++) _RenderChildren(*widget.child[i]);

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }

    void cWindowManager::Render() const
    {
      LOG<<"cWindowManager::Render"<<std::endl;
      ASSERT(pMaterial != nullptr);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();

        {
          render::ApplyMaterial apply(pMaterial);

          child_const_iterator iter = child.begin();
          child_const_iterator iterEnd = child.end();
          while (iter != iterEnd) {
            _RenderChildren(*(*iter));

            iter++;
          }
        }

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }
  }
}
