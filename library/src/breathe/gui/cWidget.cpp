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

namespace breathe
{
	namespace gui
	{
		cWidget::cWidget(unsigned int _id, WIDGET_TYPE _type, float _x, float _y, float _width, float _height) :
			pParent(NULL),
			id(_id),
			type(_type),

			x(_x),
			y(_y),
			width(_width),
			height(_height),

			bEnabled(true),
			bVisible(true),

			minimum(0),
			maximum(100),
			value(0)
		{

		}

		cWidget::~cWidget()
		{
			size_t n = child.size();
			for (size_t i = 0; i < n; i++)
				SAFE_DELETE(child[i]);
		}
			
		bool cWidget::AddChild(cWidget* pChild)
		{
			child.push_back(pChild);
			pChild->pParent = this;

			return true;
		}
		
		cWidget* cWidget::FindChild(unsigned int _id)
		{
			if (_id == id) return this;

			size_t n = child.size();
			for (size_t i = 0; i < n; i++)
				child[i]->FindChild(_id);

			return NULL;
		}

		void cWidget::SetPosition(float _x, float _y)
		{
			
		}

		void cWidget::SetSize(float _width, float _height)
		{
			
		}
	}
}
