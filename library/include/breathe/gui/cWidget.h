#ifndef CWIDGET_H
#define CWIDGET_H

namespace breathe
{
	namespace gui
	{
		enum WIDGET_TYPE
		{
			WIDGET_WINDOW = 0,
			WIDGET_BUTTON,
			WIDGET_STATICTEXT,
			WIDGET_UNKNOWN = 0xFF
		};

		// This is our base object.  
		// Windows as well as controls are derived from it.  
		// Points are all relative to the parent.  
		class cWidget
		{
		public:
			cWidget(unsigned int id, WIDGET_TYPE type, float x, float y, float width, float height);
			~cWidget();

			bool AddChild(cWidget* pChild);
			cWidget* FindChild(unsigned int id);

			unsigned int GetID() const { return id; }
			WIDGET_TYPE GetType() const { return type; }
			float GetX() const { return x; }
			float GetY() const { return y; }
			float GetWidth() const { return width; }
			float GetHeight() const { return height; }

			int GetMinimum() const { return minimum; }
			int GetMaximum() const { return maximum; }
			int GetValue() const { return value; }

			bool IsEnabled() const { return bEnabled; }
			bool IsVisible() const { return bVisible; }

			void Enable();
			void Disable();
			void Show();
			void Hide();

			void SetPosition(float x, float y);
			void SetSize(float width, float height);

		protected:
			std::vector<cWidget*> child;
			cWidget* pParent;

			unsigned int id;
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


			friend class cWindowManager;
		};

		template <WIDGET_TYPE t>
		class cWidgetTemplate : public cWidget
		{
		public:
			cWidgetTemplate(unsigned int id, float x, float y, float width, float height) :
				cWidget(id, t, x, y, width, height)
			{
			}
		};
		
		inline void cWidget::Enable()
		{
			bEnabled = true;
		}

		inline void cWidget::Disable()
		{
			bEnabled = false;
		}
		
		inline void cWidget::Show()
		{
			bVisible = true;
		}

		inline void cWidget::Hide()
		{
			bVisible = false;
		}

		typedef cWidgetTemplate<WIDGET_BUTTON> cButton;
		typedef cWidgetTemplate<WIDGET_STATICTEXT> cStaticText;
	}
}

#endif //CWIDGET_H