// Standard headers
#include <cassert>
#include <cmath>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <list>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cColour.h>

#include <spitfire/util/cString.h>

// Breathe headers
#include <breathe/gui/cManager.h>

namespace breathe
{
  namespace gui
  {
    // ** cLayoutAbsolute

    void cLayoutAbsolute::AddChild(cWidget* pChild)
    {
      children.push_back(pChild);
      positions.push_back(spitfire::math::cVec2(0.0f, 0.0f));
      widths.push_back(0.0f);
      heights.push_back(0.0f);
    }

    void cLayoutAbsolute::RemoveChildAndDestroy(cWidget* pChild)
    {
      RemoveChild(pChild);
      delete pChild;
    }

    void cLayoutAbsolute::RemoveChild(cWidget* pChild)
    {
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        // If this is the child we are looking for then remove it from the children list and remove its position, width and height
        if (pChild == children[i]) {
          children.erase(children.begin() + i);
          positions.erase(positions.begin() + i);
          widths.erase(widths.begin() + i);
          heights.erase(heights.begin() + i);
          break;
        }
      }
    }

    void cLayoutAbsolute::SetChildPositionAndSize(cWidget* pChild, const
    spitfire::math::cVec2& position0To1, float fWidth0To1, float fHeight0To1)
    {
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        // Test if this is the child we are looking for
        if (pChild == children[i]) {
            SetChildPositionAndSize(i, position0To1, fWidth0To1, fHeight0To1);
            break;
        }
      }
    }

    void cLayoutAbsolute::SetChildPositionAndSize(size_t child, const
    spitfire::math::cVec2& position0To1, float fWidth0To1, float fHeight0To1)
    {
      positions[child] = position0To1;
      widths[child] = fWidth0To1;
      heights[child] = fHeight0To1;
    }


    // ** cWidget

    cWidget::cWidget(WIDGET_TYPE _type) :
      id(0),
      type(_type),
      width(1.0f),
      height(1.0f),
      bIsDirtyRendering(true),
      pParent(nullptr),
      bIsVisible(true),
      colourBackground(0.1f, 0.1f, 0.1f),
      colourText(1.0f, 1.0f, 1.0f)
    {
    }

    cWidget::~cWidget()
    {
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(children[i]);
    }

    void cWidget::SetId(id_t _id)
    {
      id = _id;
    }

    void cWidget::SetRelativePosition(const spitfire::math::cVec2& _position)
    {
      position = _position;
    }

    spitfire::math::cVec2 cWidget::GetAbsolutePosition() const
    {
      spitfire::math::cVec2 parentPosition;
      if (pParent != nullptr) parentPosition = pParent->GetAbsolutePosition();
      return parentPosition + position;
    }

    void cWidget::AddChild(cWidget* pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(pChild->pParent == nullptr);
      children.push_back(pChild);
      pChild->pParent = this;
    }

    void cWidget::RemoveChildAndDestroy(cWidget* pChild)
    {
      RemoveChild(pChild);
      delete pChild;
    }

    void cWidget::RemoveChild(cWidget* pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(pChild->pParent == this);
      std::vector<cWidget*>::iterator iter = children.begin();
      const std::vector<cWidget*>::iterator iterEnd = children.end();
      while (iter != iterEnd) {
        if ((*iter) == pChild) {
          children.erase(iter);
          break;
        }
      }
      pChild->pParent = nullptr;
    }

    const cWidget* cWidget::GetChildById(id_t id) const
    {
      const cWidget* pChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->id == id) {
          pChild = children[i];
          break;
        }
      }

      if (pChild == nullptr) {
        for (size_t i = 0; i < n; i++) {
          pChild = children[i]->GetChildById(id);
          if (pChild != nullptr) break;
        }
      }

      return pChild;
    }

    cWidget* cWidget::GetChildById(id_t id)
    {
      cWidget* pChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->id == id) {
          pChild = children[i];
          break;
        }
      }

      if (pChild == nullptr) {
        for (size_t i = 0; i < n; i++) {
          pChild = children[i]->GetChildById(id);
          if (pChild != nullptr) break;
        }
      }

      return pChild;
    }


    // ** cLayer

    cLayer::cLayer() :
      cWidget(WIDGET_TYPE::INVISIBLE_CONTAINER)
    {
    }


    // ** cWindow

    cWindow::cWindow() :
      cWidget(WIDGET_TYPE::WINDOW)
    {
    }


    // ** cStaticText

    cStaticText::cStaticText() :
      cWidget(WIDGET_TYPE::STATICTEXT)
    {
    }


    // ** cButton

    cButton::cButton() :
      cWidget(WIDGET_TYPE::BUTTON)
    {
    }


    // ** cButton

    cInput::cInput() :
      cWidget(WIDGET_TYPE::INPUT)
    {
    }


    // ** cSlider

    cSlider::cSlider() :
      cWidget(WIDGET_TYPE::SLIDER)
    {
    }


    // ** cManager

    cManager::cManager() :
      pRoot(nullptr),
      colourWindow(0.1f, 0.1f, 0.1f, 0.6f),  // Grey
      colourWidget(0.1f, 0.1f, 0.1f, 0.8f), // Dark grey
      colourText(1.0f, 1.0f, 1.0f)           // White
    {
    }

    float cManager::GetTextHeight() const
    {
      return 0.03f;
    }

    float cManager::GetTextWidth(const spitfire::string_t& sText) const
    {
      // Just a very rough estimate
      return 0.02f * sText.length();
    }

    float cManager::GetStaticTextHeight() const
    {
      return 0.01f + GetTextHeight() + 0.01f;
    }

    float cManager::GetButtonHeight() const
    {
      return 0.01f + GetTextHeight() + 0.01f;
    }

    float cManager::GetInputHeight() const
    {
      return 0.01f + GetTextHeight() + 0.01f;
    }


    cWindow* cManager::CreateWindow()
    {
      cWindow* pWindow = new cWindow;
      pWindow->SetColour(colourWindow);
      pWindow->SetTextColour(colourText);
      return pWindow;
    }

    cStaticText* cManager::CreateStaticText()
    {
      cStaticText* pWidget = new cStaticText;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }

    cButton* cManager::CreateButton()
    {
      cButton* pWidget = new cButton;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }

    cInput* cManager::CreateInput()
    {
      cInput* pWidget = new cInput;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }

    cSlider* cManager::CreateSlider()
    {
      cSlider* pWidget = new cSlider;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }
  }
}
