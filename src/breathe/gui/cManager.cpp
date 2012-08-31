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
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

#include <spitfire/util/string.h>

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
      bIsEnabled(true),
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

        iter++;
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

    const cWidget* cWidget::FindWidgetUnderPoint(const spitfire::math::cVec2& point) const
    {
      spitfire::math::cRectangle rectangle;

      std::vector<cWidget*>::const_reverse_iterator iter = children.rbegin();
      const std::vector<cWidget*>::const_reverse_iterator iterEnd = children.rend();
      while (iter != iterEnd) {
        const cWidget& child = *(*iter);
        const spitfire::math::cVec2 position = child.GetAbsolutePosition();
        rectangle.x = position.x;
        rectangle.y = position.y;
        rectangle.width = child.width;
        rectangle.height = child.height;
        if (rectangle.ContainsPoint(point)) return child.FindWidgetUnderPoint(point);

        iter++;
      }

      const spitfire::math::cVec2 position = GetAbsolutePosition();
      rectangle.x = position.x;
      rectangle.y = position.y;
      rectangle.width = width;
      rectangle.height = height;
      if (rectangle.ContainsPoint(point)) return this;

      return nullptr;
    }

    cWidget* cWidget::FindWidgetUnderPoint(const spitfire::math::cVec2& point)
    {
      // NOTE: This must keep in line with the const version of this function
      spitfire::math::cRectangle rectangle;

      std::vector<cWidget*>::reverse_iterator iter = children.rbegin();
      const std::vector<cWidget*>::reverse_iterator iterEnd = children.rend();
      while (iter != iterEnd) {
        cWidget& child = *(*iter);
        const spitfire::math::cVec2 position = child.GetAbsolutePosition();
        spitfire::math::cRectangle rectangle;
        rectangle.x = position.x;
        rectangle.y = position.y;
        rectangle.width = child.width;
        rectangle.height = child.height;
        if (rectangle.ContainsPoint(point))return child.FindWidgetUnderPoint(point);

        iter++;
      }

      const spitfire::math::cVec2 position = GetAbsolutePosition();
      rectangle.x = position.x;
      rectangle.y = position.y;
      rectangle.width = width;
      rectangle.height = height;
      if (rectangle.ContainsPoint(point)) return this;

      return nullptr;
    }

    void cWidget::_BringChildToFront(cWidget& widget)
    {
      // If we don't have at least two children then this widget must be at the front already
      if (children.size() < 2) return;

      const size_t n = children.size() - 1;
      for (size_t i = 0; i < n; i++) {
        if (children[i] == &widget) {
          // Shift each child up
          for (; i < n; i++) children[i] = children[i + 1];

          // Now set the front most child to this widget
          children[i] = &widget;

          return;
        }
      }
    }

    void cWidget::BringToFront()
    {
      if (pParent != nullptr) pParent->_BringChildToFront(*this);
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

    void cButton::_OnEventMouseDown(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseDown button="<<button<<" at "<<x<<","<<y<<std::endl;
    }

    void cButton::_OnEventMouseUp(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseUp button="<<button<<" at "<<x<<","<<y<<std::endl;
    }

    void cButton::_OnEventMouseMove(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseMove button="<<button<<" at "<<x<<","<<y<<std::endl;
    }

    void cButton::_OnEventMouseClick(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseClick button="<<button<<" at "<<x<<","<<y<<std::endl;
    }


    // ** cInput

    cInput::cInput() :
      cWidget(WIDGET_TYPE::INPUT)
    {
    }


    // ** cSlider

    cSlider::cSlider() :
      cWidget(WIDGET_TYPE::SLIDER)
    {
    }


    // ** cRetroButton

    cRetroButton::cRetroButton()
    {
      type = WIDGET_TYPE::RETRO_BUTTON;
    }

    // ** cRetroInput

    cRetroInput::cRetroInput()
    {
      type = WIDGET_TYPE::RETRO_INPUT;
    }


    // ** cManager

    cManager::cManager() :
      pRoot(nullptr),
      idMouseLeftButtonDown(0),
      colourWindow(0.1f, 0.1f, 0.1f, 0.6f), // Grey
      colourWidget(0.1f, 0.1f, 0.1f, 0.8f), // Dark grey
      colourText(1.0f, 1.0f, 1.0f)          // White
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

    cRetroButton* cManager::CreateRetroButton()
    {
      cRetroButton* pWidget = new cRetroButton;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }

    cRetroInput* cManager::CreateRetroInput()
    {
      cRetroInput* pWidget = new cRetroInput;
      pWidget->SetColour(colourWidget);
      pWidget->SetTextColour(colourText);
      return pWidget;
    }

    const cWidget* cManager::FindWidgetUnderPoint(const spitfire::math::cVec2& point) const
    {
      ASSERT(pRoot != nullptr);
      return pRoot->FindWidgetUnderPoint(point);
    }

    cWidget* cManager::FindWidgetUnderPoint(const spitfire::math::cVec2& point)
    {
      ASSERT(pRoot != nullptr);
      return pRoot->FindWidgetUnderPoint(point);
    }

    void cManager::InjectEventMouseDown(int button, float x, float y)
    {
      std::cout<<"cManager::InjectEventMouseDown "<<x<<", "<<y<<std::endl;
      idMouseLeftButtonDown = 0;

      const spitfire::math::cVec2 point(x, y);
      cWidget* pWidget = FindWidgetUnderPoint(point);
      if (pWidget != nullptr) {
        idMouseLeftButtonDown = pWidget->GetId();
        std::cout<<"cManager::InjectEventMouseDown Sending event to "<<idMouseLeftButtonDown<<std::endl;
        pWidget->OnEventMouseDown(button, x, y);
      } else std::cout<<"cManager::InjectEventMouseDown Count not find widget to send the event to"<<std::endl;
    }

    void cManager::InjectEventMouseUp(int button, float x, float y)
    {
      //std::cout<<"cManager::InjectEventMouseUp "<<x<<", "<<y<<std::endl;
      const spitfire::math::cVec2 point(x, y);
      cWidget* pWidget = FindWidgetUnderPoint(point);
      if (pWidget != nullptr) pWidget->OnEventMouseUp(button, x, y);

      if (idMouseLeftButtonDown != 0) {
        cWidget* pWidgetLeftButtonDown = pRoot->GetChildById(idMouseLeftButtonDown);
        if ((pWidgetLeftButtonDown != nullptr) && (pWidget == pWidgetLeftButtonDown)) {
          pWidgetLeftButtonDown->OnEventMouseClick(button, x, y);
        }
        idMouseLeftButtonDown = 0;
      }
    }

    void cManager::InjectEventMouseMove(int button, float x, float y)
    {
      //std::cout<<"cManager::InjectEventMouseMove "<<x<<", "<<y<<std::endl;
      cWidget* pWidget = nullptr;
      if (idMouseLeftButtonDown != 0) pWidget = pRoot->GetChildById(idMouseLeftButtonDown);
      else {
        const spitfire::math::cVec2 point(x, y);
        pWidget = FindWidgetUnderPoint(point);
      }

      if (pWidget != nullptr) pWidget->OnEventMouseMove(button, x, y);
    }
  }
}
