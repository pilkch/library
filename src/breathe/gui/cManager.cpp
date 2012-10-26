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

// libopenglmm headers
#include <libopenglmm/cWindow.h>

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
    using opengl::KEY;

    using opengl::IsKeyPrintable;

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
      pListener(nullptr),
      id(0),
      type(_type),
      width(1.0f),
      height(1.0f),
      bIsDirtyRendering(true),
      pParent(nullptr),
      bIsVisible(true),
      bIsEnabled(true),
      bIsFocusable(false),
      bIsFocused(false),
      colourBackground(0.1f, 0.1f, 0.1f),
      colourText(1.0f, 1.0f, 1.0f)
    {
    }

    cWidget::~cWidget()
    {
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(children[i]);
    }

    void cWidget::SetEventListener(cWidgetEventListener& listener)
    {
      pListener = &listener;
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

    const cWidget* cWidget::_GetRoot() const
    {
      const cWidget* pWidget = this;
      while ((pWidget != nullptr) && (pWidget->pParent != nullptr)) pWidget = pWidget->pParent;

      return pWidget;
    }

    cWidget* cWidget::_GetRoot()
    {
      cWidget* pWidget = this;
      while ((pWidget != nullptr) && (pWidget->pParent != nullptr)) pWidget = pWidget->pParent;

      return pWidget;
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

    void cWidget::SetVisible(bool bVisible)
    {
      if (!bVisible) RemoveFocus();

      bIsVisible = bVisible;
    }

    void cWidget::RemoveFocus()
    {
      if (bIsFocused) {
        bIsFocused = false;
        return;
      }

      // Iterate down through heirarchy unsetting focus
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        children[i]->RemoveFocus();
      }
    }

    void cWidget::SetFocused()
    {
      // Iterate down through heirarchy unsetting focus
      cWidget* pWidget = _GetRoot();
      while ((pWidget != nullptr) && pWidget->bIsFocused) {
        cWidget* pChild = nullptr;
        const size_t n = pWidget->children.size();
        for (size_t i = 0; i < n; i++) {
          if (pWidget->children[i]->bIsFocused) {
            pChild = pWidget->children[i];
            break;
          }
        }

        pWidget = pChild;
      }

      // Start at this widget and iterate up through the heirarchy setting focus
      pWidget = this;
      while (pWidget != nullptr) {
        pWidget->bIsFocused = true;
        pWidget = pParent;
      }
    }

    const cWidget* cWidget::_GetFocusedChild() const
    {
      const cWidget* pFocusedChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->bIsFocused) {
          pFocusedChild = children[i];
          break;
        }
      }

      return pFocusedChild;
    }

    cWidget* cWidget::_GetFocusedChild()
    {
      cWidget* pFocusedChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->bIsFocused) {
          pFocusedChild = children[i];
          break;
        }
      }

      return pFocusedChild;
    }

    const cWidget* cWidget::_GetFocusedDescendant() const
    {
      const cWidget* pFocusedChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->bIsFocused) {
          pFocusedChild = children[i];
          break;
        }

        pFocusedChild = children[i]->_GetFocusedDescendant();
      }

      return pFocusedChild;
    }

    cWidget* cWidget::_GetFocusedDescendant()
    {
      cWidget* pFocusedChild = nullptr;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->bIsFocused) {
          pFocusedChild = children[i];
          break;
        }

        pFocusedChild = children[i]->_GetFocusedDescendant();
      }

      return pFocusedChild;
    }

    size_t cWidget::_GetFocusableChildCount() const
    {
      size_t nFocusableChildren = 0;

      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        if (children[i]->IsFocusable()) nFocusableChildren++;
      }

      return nFocusableChildren;
    }

    void cWidget::SetNextFocused()
    {
      if (pParent != nullptr) pParent->SetFocusToNextChild();
    }

    void cWidget::SetPreviousFocused()
    {
      if (pParent != nullptr) pParent->SetFocusToPreviousChild();
    }

    void cWidget::SetFocusToFirstChild()
    {
      // We are a window or invisible layer with more than 1 child, set the focus to the first focusable child
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        // Find the first focusable child
        if (children[i]->SetFocusToFirstChildRecursive()) break;
      }
    }

    bool cWidget::SetFocusToFirstChildRecursive()
    {
      // We are a window or invisible layer with more than 1 child, set the focus to the first focusable child
      const size_t n = children.size();
      for (size_t i = 0; i < n; i++) {
        // Find the first focusable child
        if (children[i]->SetFocusToFirstChildRecursive()) return true;
      }

      // We didn't find a focusable child so we set our focus to our own widget
      if (IsFocusable()) {
        bIsFocused = true;
        return true;
      }

      return false;
    }

    void cWidget::SetFocusToNextChild()
    {
      cWidget* pFocusedChild = _GetFocusedChild();

      if ((type == WIDGET_TYPE::WINDOW) || (type == WIDGET_TYPE::INVISIBLE_LAYER)) {
        if (_GetFocusableChildCount() > 1) {
          // We are a window or invisible layer with more than 1 child, set the focus to the next focusable child
          const size_t n = children.size();
          for (size_t i = 0; i < n; i++) {
            if (pFocusedChild == nullptr) {
              // We haven't had focus before so we just need to find the first focusable child
              if (children[i]->IsFocusable()) {
                children[i]->bIsFocused = true;
                break;
              }
            } else if (children[i] == pFocusedChild) {
              children[i]->bIsFocused = false;

              // Find the next child to set focus to
              i++;
              while (i < n) {
                if (children[i]->IsFocusable()) {
                  // We found the next child to set the focus to
                  children[i]->bIsFocused = true;
                  return;
                }
                i++;
              }

              // We didn't find a focusable child after this child so go back to the start and try again
              for (size_t i = 0; i < n; i++) {
                if (children[i]->IsFocusable()) {
                  // We found the next child to set the focus to
                  children[i]->bIsFocused = true;
                  return;
                }
              }
            }
          }
        }
      } else {
        const size_t n = children.size();
        for (size_t i = 0; i < n; i++) {
          if (children[i] == pFocusedChild) {
            children[i]->bIsFocused = false;

            // Find the next child to set focus to
            i++;
            for (; i < n; i++) {
              if (children[i]->IsFocusable()) {
                children[i]->bIsFocused = true;
                return;
              }
            }

            // We didn't find another focusable child after this child so tell the parent to set focus to the next child
            SetNextFocused();
          }
        }
      }
    }

    void cWidget::SetFocusToPreviousChild()
    {
      cWidget* pFocusedChild = _GetFocusedChild();

      if ((type == WIDGET_TYPE::WINDOW) || (type == WIDGET_TYPE::INVISIBLE_LAYER)) {
        if (_GetFocusableChildCount() > 1) {
          // We are a window or invisible layer with more than 1 child, set the focus to the previous focusable child
          const size_t n = children.size();
          for (size_t i = 0; i < n; i++) {
            if (pFocusedChild == nullptr) {
              // We haven't had focus before so we just need to find the first focusable child
              if (children[i]->IsFocusable()) {
                children[i]->bIsFocused = true;
                break;
              }
            } else if (children[i] == pFocusedChild) {
              children[i]->bIsFocused = false;

              // Find the previous child to set focus to
              if (i != 0) {
                i--;
                do {
                  if (children[i]->IsFocusable()) {
                    children[i]->bIsFocused = true;
                    return;
                  }

                  if (i != 0) i--;
                } while (i != 0);
              }

              // We didn't find a focusable child before this child so go back to the end and try again
              i = n - 1;
              do {
                if (children[i]->IsFocusable()) {
                  // We found the previous child to set the focus to
                  children[i]->bIsFocused = true;
                  return;
                }

                if (i != 0) i--;
              } while (i != 0);
            }
          }
        }
      } else {
        const size_t n = children.size();
        for (size_t i = 0; i < n; i++) {
          if (children[i] == pFocusedChild) {
            children[i]->bIsFocused = false;

            // Find the previous child to set focus to
            if (i != 0) {
              i--;
              do {
                if (children[i]->IsFocusable()) {
                  children[i]->bIsFocused = true;
                  return;
                }

                if (i != 0) i--;
              } while (i != 0);
            }

            // We didn't find another focusable child after this child so tell the parent to set focus to the previous child
            SetPreviousFocused();
          }
        }
      }
    }


    // ** cLayer

    cLayer::cLayer() :
      cWidget(WIDGET_TYPE::INVISIBLE_LAYER)
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
      bIsFocusable = true;
    }

    EVENT_RESULT cButton::_OnEventMouseDown(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseDown button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cButton::_OnEventMouseUp(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseUp button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cButton::_OnEventMouseMove(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseMove button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cButton::_OnEventMouseClick(int button, float x, float y)
    {
      std::cout<<"cButton::_OnEventMouseClick button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cCheckbox

    cCheckbox::cCheckbox() :
      cWidget(WIDGET_TYPE::CHECKBOX),
      bIsSelected(false)
    {
      bIsFocusable = true;
    }

    bool cCheckbox::IsSelected() const
    {
      return bIsSelected;
    }

    void cCheckbox::SetSelected(bool bSelected)
    {
      bIsSelected = true;
    }

    EVENT_RESULT cCheckbox::_OnEventMouseDown(int button, float x, float y)
    {
      std::cout<<"cCheckbox::_OnEventMouseDown button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cCheckbox::_OnEventMouseUp(int button, float x, float y)
    {
      std::cout<<"cCheckbox::_OnEventMouseUp button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cCheckbox::_OnEventMouseMove(int button, float x, float y)
    {
      std::cout<<"cCheckbox::_OnEventMouseMove button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }

    EVENT_RESULT cCheckbox::_OnEventMouseClick(int button, float x, float y)
    {
      std::cout<<"cCheckbox::_OnEventMouseClick button="<<button<<" at "<<x<<","<<y<<std::endl;
      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cInput

    cInput::cInput() :
      cWidget(WIDGET_TYPE::INPUT)
    {
      bIsFocusable = true;
    }


    // ** cSlider

    cSlider::cSlider() :
      cWidget(WIDGET_TYPE::SLIDER)
    {
      bIsFocusable = true;
    }


    // ** cRetroButton

    cRetroButton::cRetroButton()
    {
      type = WIDGET_TYPE::RETRO_BUTTON;
    }

    EVENT_RESULT cRetroButton::_OnEventKeyboardDown(int keyCode)
    {
      if (keyCode == KEY::RETURN) {
        if (pListener != nullptr) {
          cWidgetEvent event;
          event.SetWidget(this);
          event.SetType(cWidgetEvent::TYPE::PRESSED);
          pListener->_OnWidgetEvent(event);
        }
        return EVENT_RESULT::HANDLED;
      }

      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cRetroInput

    cRetroInput::cRetroInput()
    {
      type = WIDGET_TYPE::RETRO_INPUT;
    }

    EVENT_RESULT cRetroInput::_OnEventKeyboardDown(int keyCode)
    {
      switch (keyCode) {
        case KEY::BACKSPACE: {
          if (!sCaption.empty()) {
            // Strip the last character
            sCaption = sCaption.substr(0, sCaption.length() - 1);
          }
          return EVENT_RESULT::HANDLED;
        }
        default: {
          if (IsKeyPrintable(KEY(keyCode))) {
            const char_t szText[2] = { char_t(keyCode), 0 };
            sCaption.append(szText);
            return EVENT_RESULT::HANDLED;
          }
        }
      };

      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cRetroInputUpDown

    cRetroInputUpDown::cRetroInputUpDown() :
      min(0),
      max(100),
      value(0)
    {
      type = WIDGET_TYPE::RETRO_INPUT_UPDOWN;

      SetValue(0, false);
    }

    void cRetroInputUpDown::SetRange(int _min, int _max)
    {
      min = _min;
      max = _max;
    }

    void cRetroInputUpDown::SetValue(int _value, bool bNotifyListener)
    {
      if ((_value >= min) && (_value <= max)) {
        value = _value;
        SetCaption(spitfire::string::ToString(value));
      }
    }

    EVENT_RESULT cRetroInputUpDown::_OnEventKeyboardDown(int keyCode)
    {
      switch (keyCode) {
        case KEY::LEFT: {
          SetValue(value - 1, true);
          return EVENT_RESULT::HANDLED;
        }
        case KEY::RIGHT: {
          SetValue(value + 1, true);
          return EVENT_RESULT::HANDLED;
        }
      }

      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cRetroColourPicker

    cRetroColourPicker::cRetroColourPicker() :
      selected(0)
    {
      type = WIDGET_TYPE::RETRO_COLOUR_PICKER;

      SetSelectedColour(0, false);
    }

    size_t cRetroColourPicker::GetNumberOfColours() const
    {
      return colours.size();
    }

    string_t cRetroColourPicker::GetColourName(size_t index) const
    {
      if (index < colours.size()) return colourNames[index];

      return TEXT("");
    }

    spitfire::math::cColour cRetroColourPicker::GetColour(size_t index) const
    {
      if (index < colours.size()) return colours[index];

      return spitfire::math::cColour(0.0f, 0.0f, 0.0f);
    }

    void cRetroColourPicker::AddColour(const string_t& sName, const spitfire::math::cColour& colour)
    {
      colourNames.push_back(sName);
      colours.push_back(colour);
    }

    void cRetroColourPicker::SetSelectedColour(size_t index, bool bNotifyListener)
    {
      if (index < colours.size()) {
        size_t previous = selected;

        selected = index;

        // Give the listener a chance to veto the action
        if (bNotifyListener && (pListener != nullptr)) {
          cWidgetEvent event;
          event.SetWidget(this);
          event.SetType(cWidgetEvent::TYPE::CHANGED);
          if (pListener->_OnWidgetEvent(event) == EVENT_RESULT::HANDLED) {
            // The event has been vetoed
            selected = previous;
            return;
          }
        }

      }
    }

    EVENT_RESULT cRetroColourPicker::_OnEventKeyboardDown(int keyCode)
    {
      switch (keyCode) {
        case KEY::LEFT: {
          int index = int(selected);
          SetSelectedColour(index - 1, true);
          return EVENT_RESULT::HANDLED;
        }
        case KEY::RIGHT: {
          int index = int(selected);
          SetSelectedColour(index + 1, true);
          return EVENT_RESULT::HANDLED;
        }
      }

      return EVENT_RESULT::NOT_HANDLED_PERCOLATE;
    }


    // ** cWidgetEvent

    cWidgetEvent::cWidgetEvent() :
      pWidget(nullptr),
      type(TYPE::UNKNOWN)
    {
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

    float cManager::GetCheckboxHeight() const
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

    cCheckbox* cManager::CreateCheckbox()
    {
      cCheckbox* pWidget = new cCheckbox;
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

    const cWidget* cManager::GetFocusedWidget() const
    {
      return pRoot->_GetFocusedDescendant();
    }

    cWidget* cManager::GetFocusedWidget()
    {
      return pRoot->_GetFocusedDescendant();
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

    bool cManager::InjectEventKeyboardDown(int keyCode)
    {
      ASSERT(pRoot != nullptr);
      cWidget* pWidget = GetFocusedWidget();
      if (pWidget != nullptr) {
        std::cout<<"cManager::InjectEventKeyboardDown Sending event to "<<pWidget->GetId()<<std::endl;
        if (pWidget->OnEventKeyboardDown(keyCode) == EVENT_RESULT::HANDLED) return true;
      } else std::cout<<"cManager::InjectEventKeyboardDown Could not find widget to send the event to"<<std::endl;

      switch (keyCode) {
        case KEY::DOWN: {
          if (pWidget != nullptr) {
            pWidget->SetNextFocused();
            return true;
          }

          break;
        }
        case KEY::UP: {
          if (pWidget != nullptr) {
            pWidget->SetPreviousFocused();
            return true;
          }

          break;
        }
      }

      return false;
    }

    bool cManager::InjectEventKeyboardUp(int keyCode)
    {
      ASSERT(pRoot != nullptr);
      cWidget* pWidget = GetFocusedWidget();
      if (pWidget != nullptr) {
        std::cout<<"cManager::InjectEventKeyboardUp Sending event to "<<pWidget->GetId()<<std::endl;
        if (pWidget->OnEventKeyboardUp(keyCode) == EVENT_RESULT::HANDLED) return true;
      } else std::cout<<"cManager::InjectEventKeyboardUp Could not find widget to send the event to"<<std::endl;

      return false;
    }

    bool cManager::InjectEventMouseDown(int button, float x, float y)
    {
      std::cout<<"cManager::InjectEventMouseDown "<<x<<", "<<y<<std::endl;
      idMouseLeftButtonDown = 0;

      const spitfire::math::cVec2 point(x, y);
      cWidget* pWidget = FindWidgetUnderPoint(point);
      if (pWidget != nullptr) {
        idMouseLeftButtonDown = pWidget->GetId();
        std::cout<<"cManager::InjectEventMouseDown Sending event to "<<idMouseLeftButtonDown<<std::endl;
        return (pWidget->OnEventMouseDown(button, x, y) == EVENT_RESULT::HANDLED);
      } else std::cout<<"cManager::InjectEventMouseDown Could not find widget to send the event to"<<std::endl;

      return false;
    }

    bool cManager::InjectEventMouseUp(int button, float x, float y)
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

      return false;
    }

    bool cManager::InjectEventMouseMove(int button, float x, float y)
    {
      //std::cout<<"cManager::InjectEventMouseMove "<<x<<", "<<y<<std::endl;
      cWidget* pWidget = nullptr;
      if (idMouseLeftButtonDown != 0) pWidget = pRoot->GetChildById(idMouseLeftButtonDown);
      else {
        const spitfire::math::cVec2 point(x, y);
        pWidget = FindWidgetUnderPoint(point);
      }

      if (pWidget != nullptr) return (pWidget->OnEventMouseMove(button, x, y) == EVENT_RESULT::HANDLED);

      return false;
    }
  }
}
