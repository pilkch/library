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
#include <GL/Glee.h>
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

#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindow.h>
#include <breathe/gui/cWindowManager.h>

namespace breathe
{
	namespace gui
	{			
		render::material::cMaterial* pMaterial = nullptr;
		std::vector<render::cTexture*> textureBackground;

		enum
		{
			BACKGROUND_NORMAL,
			BACKGROUND_DARKER,
			BACKGROUND_DARKEST,
			BACKGROUND_TEXT
		};

		cWindowManager::cWindowManager()
		{
		}

		cWindowManager::~cWindowManager()
		{
			size_t n = child.size();
			for (size_t i = 0; i < n; i++)
				SAFE_DELETE(child[i]);
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
		}
		
		void cWindowManager::OnMouseEvent(int button, int state, int x, int y)
		{
			/*if(button == 5 && state == SDL_PRESSED)
				pPlayer->ChangeItemUp();

			if(button == 4 && state == SDL_PRESSED)
				pPlayer->ChangeItemDown();*/
		}

		void cWindowManager::Update(sampletime_t currentTime)
		{
			//if ()
		}
			
		bool cWindowManager::AddChild(cWindow* pChild)
		{
			child.push_back(pChild);
			pChild->pParent = nullptr;
			
			return true;
		}

		void cWindowManager::_RenderWindow(const cWidget& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.HorizontalRelativeToAbsolute(widget.GetHeight());
			
			const float bar_height = 0.02f;
			const float bar_v = 0.04f;

			// Draw the top left corner
			pRender->RenderScreenSpaceRectangle(
				widget.GetX(), widget.GetY(), 0.02f, bar_height,
				0.0f, 0.0f, 0.05f, bar_v);
			
			// Draw the top right corner
			pRender->RenderScreenSpaceRectangle(
				widget.GetX() + absolute_width - 0.016f, widget.GetY(), 0.016f, bar_height,
				0.187f, 0.0f, 0.049f, bar_v);

			// Draw the top bar
			pRender->RenderScreenSpaceRectangle(
				widget.GetX() + 0.02f, widget.GetY(), absolute_width - 0.02f - 0.01f, bar_height,
				0.18f, 0.0f, 0.01f, bar_v);
			
			// TODO: Draw the caption

			// TODO: Draw the close button
						
			// TODO: Draw the minimise and maximise buttons

			if (widget.IsResizable()) {
				// TODO: Draw the bottom right corner
				//pRender->RenderScreenSpaceRectangle(
				//	widget.GetX() + 0.02f, widget.GetY(), absolute_width - 0.02f - 0.01f, 0.05f,
				//	0.18f, 0.0f, 0.01f, 0.1f);
			}

			// Draw the window background
			float width = absolute_width;
			float height = absolute_height - bar_height;
			render::ApplyTexture apply(textureBackground[BACKGROUND_NORMAL]);
			pRender->RenderScreenSpaceRectangle(
				widget.GetX(), widget.GetY() + bar_height, absolute_width, absolute_height - bar_height,
				0.0f, 0.0f, width / 64.0f, height / 64.0f);
		}

		void cWindowManager::_RenderInput(const cWidget& widget)
		{
			const float absolute_width = widget.HorizontalRelativeToAbsolute(widget.GetWidth());
			const float absolute_height = widget.HorizontalRelativeToAbsolute(widget.GetHeight());
			
			render::ApplyTexture apply(textureBackground[BACKGROUND_TEXT]);
			pRender->RenderScreenSpaceRectangle(
				widget.HorizontalRelativeToAbsolute(widget.GetX()), widget.HorizontalRelativeToAbsolute(widget.GetY()), 
				absolute_width, absolute_height,
				0.0f, 0.0f, absolute_width / 64.0f, absolute_height / 64.0f);
		}

		void cWindowManager::_RenderWidget(const cWidget& widget)
		{
			switch(widget.GetType())
			{
				case WIDGET_WINDOW:
					_RenderWindow(widget);
					break;

				case WIDGET_INPUT:
					_RenderInput(widget);
					break;
					
				default:
					pRender->RenderScreenSpaceRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());
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

			pRender->SetMaterial(pMaterial);
			
			// Setup texture matrix
			pRender->SelectTextureUnit0();
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glLoadIdentity();
			glScalef(1.0f, -1.0f, 1.0f);
			glMatrixMode( GL_MODELVIEW );

			size_t n = child.size();
			for (size_t i = 0; i < n; i++)
				_RenderChildren(*child[i]);

			glMatrixMode( GL_TEXTURE );			// Select Texture
			glPopMatrix();									// Pop The Matrix
		}
	}
}
