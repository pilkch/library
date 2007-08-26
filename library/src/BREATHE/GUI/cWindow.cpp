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
#include <BREATHE/GUI/cWindow.h>

namespace BREATHE
{
	namespace GUI
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
