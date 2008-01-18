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

			~cWindowManager();

			void LoadTheme();
			void Render();
			
			void OnMouseEvent(int button, int state, int x, int y);
			void Update(sampletime_t currentTime);
			
			bool AddChild(cWindow* pChild);
			bool RemoveChild(cWindow* pChild);

		private:
			void _RenderChildren(const cWidget& widget);
			void _RenderWidget(const cWidget& widget);

			void _RenderWindow(const cWindow& widget);
			void _RenderButton(const cWidget& widget);
			void _RenderStaticText(const cWidget& widget);
			void _RenderInput(const cWidget& widget);

      child_list child;
		};
	}
}

#endif //CWINDOW_MANAGER_H
