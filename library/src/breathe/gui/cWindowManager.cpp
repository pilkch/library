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

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

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
#include <breathe/gui/cWindowManager.h>

const float fTextureWidth = 64.0f;
const float fOneOverTextureWidth = 1.0f / fTextureWidth;

float CreateTextureCoord(float value) { return value * fOneOverTextureWidth; }

namespace breathe
{
	namespace gui
	{
		render::material::cMaterial* pMaterial = nullptr;
		std::vector<render::cTexture*> textureBackground;
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

			assert(pMaterial != nullptr);
			assert(textureBackground[0] != nullptr);
			assert(textureBackground[1] != nullptr);
			assert(textureBackground[2] != nullptr);
			assert(textureBackground[3] != nullptr);

			pFontWindowCaption = new render::cFont(TEXT("osx_fonts/Lucida Grande.ttf"), 10);
		}

    cWindow* cWindowManager::_FindWindowUnderPoint(float x, float y)
    {
      cWindow* pWindow = nullptr;
      child_iterator iter = child.begin();
      child_iterator iterEnd = child.end();
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

      return false;
		}

		void cWindowManager::Update(sampletime_t currentTime)
		{
			//if ()
		}

		bool cWindowManager::AddChild(cWindow* pChild)
		{
      assert(pChild != nullptr);
			child.push_back(pChild);
			pChild->pParent = nullptr;

			return true;
		}

    bool cWindowManager::RemoveChild(cWindow* pChild)
    {
      assert(pChild != nullptr);
			child.remove(pChild);
      SAFE_DELETE(pChild);

      return true;
    }

		void cWindowManager::_RenderWindow(const cWindow& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

			const float bar_height = 0.02f;
			const float bar_v = 0.04f;

      // Draw the top left corner
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.GetX(), widget.GetY(), 0.02f, bar_height,
				0.0f, 0.0f, 0.05f, bar_v);

			// Draw the top right corner
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.GetX() + absolute_width - 0.016f, widget.GetY(), 0.016f, bar_height,
				0.187f, 0.0f, 0.049f, bar_v);

			// Draw the top bar
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.GetX() + 0.02f, widget.GetY(), absolute_width - 0.02f - 0.01f, bar_height,
				0.18f, 0.0f, 0.01f, bar_v);

			// TODO: Draw the caption

			// TODO: Draw the close button

			// TODO: Draw the minimise and maximise buttons

			if (widget.IsResizable()) {
				// TODO: Draw the bottom right corner
				//pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				//	widget.GetX() + 0.02f, widget.GetY(), absolute_width - 0.02f - 0.01f, 0.05f,
				//	0.18f, 0.0f, 0.01f, 0.1f);
			}

			// Draw the window background
			float width = absolute_width;
			float height = absolute_height - bar_height;
			render::ApplyTexture apply(textureBackground[BACKGROUND_NORMAL]);
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.GetX(), widget.GetY() + bar_height, absolute_width, absolute_height - bar_height,
				0.0f, 0.0f, CreateTextureCoord(width), CreateTextureCoord(height));

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

						//pFontWindowCaption->printfCenteredHorizontallyVertically(widget.GetX(), widget.GetY(),
						//	widget.GetWidth(), bar_height,
						//	breathe::string::ToUTF8(widget.GetCaption()).c_str());
						pFontWindowCaption->PrintCenteredHorizontally(widget.GetX(), widget.GetY(),
							widget.GetWidth(), widget.GetCaption());

						glMatrixMode(GL_TEXTURE);
					glPopMatrix();

          pRender->SetMaterial(pMaterial);
				glPopAttrib();
			}
		}

		void cWindowManager::_RenderInput(const cWidget& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

			render::ApplyTexture apply(textureBackground[BACKGROUND_TEXT]);
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.HorizontalRelativeToAbsolute(widget.GetX()), widget.VerticalRelativeToAbsolute(widget.GetY()),
				absolute_width, absolute_height,
				0.0f, 0.0f, CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));
		}

		void cWindowManager::_RenderButton(const cWidget& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.HorizontalRelativeToAbsolute(widget.GetX()), widget.VerticalRelativeToAbsolute(widget.GetY()),
				absolute_width, absolute_height,
				0.0083f, 0.073f, 0.08f, 0.045f);//CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));

      // Draw the text of this widget
			glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
				pRender->ClearMaterial();

				pRender->SelectTextureUnit0();

				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
					glLoadIdentity();

					glMatrixMode(GL_MODELVIEW);

					glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

					//pFontWindowCaption->printfCenteredHorizontallyVertically(widget.GetX(), widget.GetY(),
					//	widget.GetWidth(), bar_height,
					//	breathe::string::ToUTF8(widget.GetCaption()).c_str());
					pFontWindowCaption->PrintCenteredHorizontally(
            widget.HorizontalRelativeToAbsolute(widget.GetX()), widget.VerticalRelativeToAbsolute(widget.GetY()),
						widget.HorizontalRelativeToAbsolute(widget.GetWidth()),
            widget.GetText());

					glMatrixMode(GL_TEXTURE);
				glPopMatrix();

        pRender->SetMaterial(pMaterial);
			glPopAttrib();
		}

		/*void cWindowManager::_RenderStaticText(const cWidget& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.VerticalRelativeToAbsolute(widget.GetHeight());

			render::ApplyTexture apply(textureBackground[BACKGROUND_TEXT]);
			pRender->RenderScreenSpaceRectangleTopLeftIsAt(
				widget.HorizontalRelativeToAbsolute(widget.GetX()), widget.VerticalRelativeToAbsolute(widget.GetY()),
				absolute_width, absolute_height,
				0.0f, 0.0f, CreateTextureCoord(absolute_width), CreateTextureCoord(absolute_height));

      pRender->BeginRenderingText();
			  breathe::constant_stack<std::string>::reverse_iterator iter = CONSOLE.rbegin();
			  breathe::constant_stack<std::string>::reverse_iterator iterEnd = CONSOLE.rend();
			  unsigned int y = 60;
			  while(iter != iterEnd)
			  {
				  pFont->printf(0, static_cast<float>(y), (*iter).c_str());
				  y += 30;

				  iter++;
			  };
      pRender->EndRenderingText();
		}*/

		void cWindowManager::_RenderWidget(const cWidget& widget)
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
					//_RenderStaticText(widget);
					break;

				default:
					;
					//pRender->RenderScreenSpaceRectangleTopLeftIsAt(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());
			};
		}

		void cWindowManager::_RenderChildren(const cWidget& widget)
		{
			if (false == widget.IsVisible()) return;

			_RenderWidget(widget);

			size_t n = widget.child.size();
			if (n == 0) return;

			pRender->PushScreenSpacePosition(widget.GetX(), widget.GetY());

				for (size_t i = 0; i < n; i++)
					_RenderChildren(*widget.child[i]);

			pRender->PopScreenSpacePosition();
		}

		void cWindowManager::Render()
		{
			assert(pMaterial != nullptr);

			pRender->ClearMaterial();
			pRender->SetMaterial(pMaterial);

			// Setup texture matrix
			pRender->SelectTextureUnit0();
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
				glLoadIdentity();
				//glScalef(1.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
			  glPushMatrix();

          child_const_iterator iter = child.begin();
          child_const_iterator iterEnd = child.end();
          while (iter != iterEnd) {
            _RenderChildren(*(*iter));

            iter++;
          }

				  glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

				glMatrixMode(GL_TEXTURE);
			glPopMatrix();
		}
	}
}
