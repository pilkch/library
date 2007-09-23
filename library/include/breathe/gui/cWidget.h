#ifndef CWIDGET_H
#define CWIDGET_H

namespace breathe
{
	namespace GUI
	{
		// This is our base object.  
		// Windows as well as controls are derived from it.  
		// Points are all relative to the parent.  
		class cWidget
		{
		public:
			cWidget(unsigned int id, float x, float y, float width, float height);
			~cWidget();

			bool AddChild(cWidget* pChild);

			unsigned int GetID();
			float GetX();
			float GetY();

			void Enable();
			void Disable();
			void Show();
			void Hide();

			
			virtual void Render();

			void SetPosition(float x, float y);
			void SetSize(float width, float height);

		protected:
			std::vector<cWidget*> child;
			cWidget* pParent;

			unsigned int id;

			float x;
			float y;

      float width;
			float height;

			bool bEnabled;
			bool bVisible;


			void RenderChildren();
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
	}
}

#endif //CWIDGET_H