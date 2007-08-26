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
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/GUI/cWidget.h>

namespace BREATHE
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
