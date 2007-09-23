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
			cWidget(_id, _x, _y, _width, _height)
		{

		}

		cWindow::~cWindow()
		{

		}

		

		cWindowManager::cWindowManager(float _x, float _y, float _width, float _height) :
				cWidget(0, _x, _y, _width, _height)
		{

		}

		cWindowManager::~cWindowManager()
		{

		}

		void cWindowManager::Render()
		{
			if (false == bVisible) return;

			RenderChildren();
		}
	}
}
