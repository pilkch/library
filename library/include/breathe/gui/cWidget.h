#ifndef CWIDGET_H
#define CWIDGET_H

namespace breathe
{
	namespace gui
	{
		enum WIDGET_TYPE
		{
			WIDGET_UNKNOWN = 0,
			WIDGET_WINDOW,
			WIDGET_BUTTON,
			WIDGET_STATICTEXT,
			WIDGET_INPUT
		};

		// This is our base object.  
		// Windows as well as controls are derived from it.  
		// Points are all relative to the parent.  
		class cWidget
		{
		public:
			cWidget(unsigned int idControl, WIDGET_TYPE type, float x, float y, float width, float height);
			virtual ~cWidget();

			bool AddChild(cWidget* pChild);
			cWidget* FindChild(unsigned int idControl);

			unsigned int GetID() const { return idControl; }
			WIDGET_TYPE GetType() const { return type; }
			
			float HorizontalRelativeToAbsolute(float n) const;
			float VerticalRelativeToAbsolute(float n) const;

			float GetX() const { return x; }
			float GetY() const { return y; }
			float GetWidth() const { return width; }
			float GetHeight() const { return height; }

			int GetMinimum() const { return minimum; }
			int GetMaximum() const { return maximum; }
			int GetValue() const { return value; }

			bool IsEnabled() const;
			bool IsVisible() const;
			bool IsResizable() const { return bResizable; }

			void Enable() { bEnabled = true; }
			void Disable() { bEnabled = false; }
			void Show() { bVisible = true; }
			void Hide() { bVisible = false; }
			
			void SetResizable() { bResizable = true; }
			void SetFixedSize() { bResizable = false; }
			void SetPosition(float x, float y);
			void SetSize(float width, float height);

		protected:
			std::vector<cWidget*> child;
			cWidget* pParent;

			unsigned int idControl;
			WIDGET_TYPE type;

			int minimum;
			int maximum;
			int value;

			float x;
			float y;
			
      float width;
			float height;

			bool bEnabled;
			bool bVisible;
			bool bResizable;


			friend class cWindowManager;
		};
				
		template <WIDGET_TYPE t>
		class cWidgetTemplate : public cWidget
		{
		public:
			cWidgetTemplate(unsigned int idControl, float x, float y, float width, float height) :
				cWidget(idControl, t, x, y, width, height)
			{
			}
		};
		
		typedef cWidgetTemplate<WIDGET_BUTTON> cWidget_Button;
		typedef cWidgetTemplate<WIDGET_STATICTEXT> cWidget_StaticText;
		typedef cWidgetTemplate<WIDGET_INPUT> cWidget_Input;
	}
}

#endif //CWIDGET_H