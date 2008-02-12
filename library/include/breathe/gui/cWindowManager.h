#ifndef CWINDOW_MANAGER_H
#define CWINDOW_MANAGER_H

// TODO: include shadows for gui with an alpha blend niceynicey.  

namespace breathe
{
	namespace gui
	{
		class cWindowManager
		{
		public:
      typedef std::list<cWindow*> child_list;
      typedef child_list::iterator child_iterator;
      typedef child_list::const_iterator child_const_iterator;

      cWindowManager();
			~cWindowManager();

			void LoadTheme();
			void Render();
			
			void OnMouseEvent(int button, int state, float x, float y);
			void Update(sampletime_t currentTime);
			
			bool AddChild(cWindow* pChild);
			bool RemoveChild(cWindow* pChild);

		private:
      cWindow* _FindWindowUnderPoint(float x, float y);

			void _RenderChildren(const cWidget& widget);
			void _RenderWidget(const cWidget& widget);

			void _RenderWindow(const cWindow& widget);
			void _RenderButton(const cWidget& widget);
			void _RenderStaticText(const cWidget& widget);
			void _RenderInput(const cWidget& widget);

      child_list child;
      cWidget* pEventWidgetMouseLeftButtonDown;
		};
	}
}

#endif //CWINDOW_MANAGER_H
