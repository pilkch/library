#ifndef CWINDOW_MANAGER_H
#define CWINDOW_MANAGER_H

/*
include shadows for gui with an alpha blend niceynicey.
*/

namespace breathe
{
	namespace gui
	{
		unsigned int GenerateID();

		class cWindowManager
		{
		public:
			cWindowManager();
			~cWindowManager();

			void LoadTheme();
			void Render();

			// Handles events such as clicking on a button, listview selection changed, 
			// pulldownlist selection changed, input box keypress, etc.
			virtual void DefaultEvent(unsigned int id) {}

			bool AddChild(cWindow* pChild);

		private:
			void _RenderChildren(const cWidget& widget);
			void _RenderWidget(const cWidget& widget);

			void _RenderWindow(const cWidget& widget);
			void _RenderButton(const cWidget& widget);
			void _RenderStaticText(const cWidget& widget);
			
			std::vector<cWindow*> child;
		};
	}
}

#endif //CWINDOW_MANAGER_H