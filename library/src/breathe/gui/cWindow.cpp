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
    // *** cWindow

    cWindow::cWindow(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* _pParent, bool _bModeless, bool _bResizable) :
      cWidget(id, WIDGET_WINDOW, x, y, width, height),
      z(0)
    {
      SetCaption(caption);
      SetParent(_pParent);

      if (pParent != nullptr) z = ((cWindow*)pParent)->GetZDepth() + 1;

      bModeless = _bModeless;
      bResizable = _bResizable;
    }

    void cWindow::Update(sampletime_t currentTime)
    {
    }

    void cWindow::OnMouseEvent(int button, int state, float x, float y)
    {
      if (button == SDL_BUTTON_LEFT) {
        cWidget* p = FindChildAtPoint(x, y);

        while (p != nullptr && (!p->IsVisible() || !p->IsEnabled())) {
          p = p->GetParent();
        }

        if (p != nullptr) {
          if (state != 0) p->OnLeftMouseDown(x, y);
          else p->OnLeftMouseUp(x, y);
          std::cout<<breathe::string::ToUTF8(p->GetText());
        } else std::cout<<breathe::string::ToUTF8(GetText());

        std::cout<<" "<<x<<","<<y<<std::endl;
      }

      _OnMouseEvent(button, state, x, y);
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
