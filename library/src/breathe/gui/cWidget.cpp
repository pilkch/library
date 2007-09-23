#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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
#include <breathe/util/log.h>
#include <breathe/util/cFileSystem.h>

#include <breathe/math/cMath.h>
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

namespace breathe
{
	namespace GUI
	{
		cWidget::cWidget(unsigned int _id, float _x, float _y, float _width, float _height) :
			pParent(NULL),
			id(_id),
			x(_x),
			y(_y),
			width(_width),
			height(_height),
			bEnabled(true),
			bVisible(true)
		{

		}

		cWidget::~cWidget()
		{
			unsigned int n = child.size();
			for (unsigned int i = 0; i < n; i++)
				SAFE_DELETE(child[i]);
		}
			
		bool cWidget::AddChild(cWidget* pChild)
		{
			child.push_back(pChild);
			pChild->pParent = this;

			return true;
		}
		
		void cWidget::Render()
		{
			if (false == bVisible) return;

			pRender->RenderScreenSpaceRectangle(x, y, width, height);

			RenderChildren();
		}

		void cWidget::RenderChildren()
		{
			pRender->PushScreenSpacePosition(x, y);

				unsigned int n = child.size();
				for (unsigned int i = 0; i < n; i++)
					child[i]->Render();

			pRender->PopScreenSpacePosition();
		}

		void cWidget::SetPosition(float _x, float _y)
		{
			
		}

		void cWidget::SetSize(float _width, float _height)
		{
			
		}
	}
}
