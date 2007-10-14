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
#include <breathe/util/filesystem.h>

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

namespace breathe
{
	namespace gui
	{
		cWindow::cWindow(unsigned int _id, float _x, float _y, float _width, float _height) :
			cWidget(_id, WIDGET_WINDOW, _x, _y, _width, _height)
		{
		}

		cWindow::~cWindow()
		{
		}

		
		render::material::cMaterial* pMaterial= NULL;

		cWindowManager::cWindowManager()
		{
		}

		cWindowManager::~cWindowManager()
		{
			unsigned int n = child.size();
			for (unsigned int i = 0; i < n; i++)
				SAFE_DELETE(child[i]);
		}

		void cWindowManager::LoadTheme()
		{
			pMaterial = pRender->AddMaterial("gui.mat");
		}
			
		bool cWindowManager::AddChild(cWindow* pChild)
		{
			child.push_back(pChild);
			pChild->pParent = nullptr;

			return true;
		}

		void cWindowManager::RenderChildren(const cWidget& widget)
		{
			if (false == widget.IsVisible()) return;			

			pRender->RenderScreenSpaceRectangle(widget.GetX(), widget.GetY(), widget.GetWidth(), widget.GetHeight());

			unsigned int n = widget.child.size();
			if (n == 0) return;

			pRender->PushScreenSpacePosition(widget.GetX(), widget.GetY());
				
				for (unsigned int i = 0; i < n; i++)
					RenderChildren(*widget.child[i]);

			pRender->PopScreenSpacePosition();
		}

		void cWindowManager::Render()
		{
			assert(pMaterial != nullptr);

			pRender->SetMaterial(pMaterial);

			unsigned int n = child.size();
			for (unsigned int i = 0; i < n; i++)
				RenderChildren(*child[i]);
		}
	}
}
