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

namespace breathe
{
	namespace gui
	{
		id_t nextID = 1;

		id_t GenerateID()
		{
			return nextID++;
		}

		cWidget::cWidget(id_t _idControl, WIDGET_TYPE _type, float _x, float _y, float _width, float _height) :
			pParent(NULL),
			idControl(_idControl),
			type(_type),

			x(_x),
			y(_y),
			width(_width),
			height(_height),

			bEnabled(true),
			bVisible(true),
			bResizable(false),

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
		
		cWidget* cWidget::FindChild(id_t _idControl)
		{
			if (_idControl == idControl) return this;

			size_t n = child.size();
			cWidget* p = nullptr;
			for (size_t i = 0; i < n; i++) {
				p = child[i]->FindChild(_idControl);
				if (p != nullptr) return p;
			}

			return nullptr;
		}

    cWidget* cWidget::FindChildAtPoint(float _x, float _y)
    {
			size_t n = child.size();
			cWidget* p = nullptr;
			for (size_t i = 0; i < n; i++) {
        p = child[i];
        if (math::PointIsWithinBounds(_x, _y, 
          p->GetX(), p->GetY(), p->GetWidth(), p->GetHeight())) return p->FindChildAtPoint(_x, _y);
			}

			return this;
    }
		
		bool cWidget::IsEnabled() const
		{
			if (pParent != nullptr) return pParent->IsEnabled() && bEnabled;
			return bEnabled;
		}

		bool cWidget::IsVisible() const
		{
			if (pParent != nullptr) return pParent->IsVisible() && bVisible;
			return bVisible;
		}
		
		float cWidget::HorizontalRelativeToAbsolute(float n) const
		{
			if (pParent != nullptr) return pParent->GetX() + (n * pParent->HorizontalRelativeToAbsolute(pParent->GetWidth()));
			return n;
		}
		
		float cWidget::VerticalRelativeToAbsolute(float n) const
		{
			if (pParent != nullptr) return pParent->GetY() + (n * pParent->VerticalRelativeToAbsolute(pParent->GetHeight()));
			return n;
		}

		void cWidget::SetPosition(float _x, float _y)
		{
			
		}

		void cWidget::SetSize(float _width, float _height)
		{
			
		}


		/*
		cWidget_Input
		
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		std::string currentline(CONSOLE.GetCurrentLine());
		if (CONSOLE.uiCursorBlink>20) pFont->printf(0, 20, currentline.c_str());
		else
			pFont->printf(0, 20, (
				currentline.substr(0, CONSOLE.uiCursorPosition) + "|" + 
				currentline.substr(CONSOLE.uiCursorPosition)).c_str());
		*/
	}
}
