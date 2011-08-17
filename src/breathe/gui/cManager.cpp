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
      pParent(nullptr)
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


    // ** cManager

    cManager::cManager() :
      pRoot(nullptr)
    {
    }
  }
}
