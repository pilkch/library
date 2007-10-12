#ifndef CWINDOW_H
#define CWINDOW_H

/*
include shadows for gui with an alpha blend niceynicey.
*/

namespace breathe
{
	namespace gui
	{
		class cWindow : public cWidget
		{
		public:
			cWindow(unsigned int id, float x, float y, float width, float height);
			~cWindow();
		};

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
			void RenderChildren(const cWidget& widget);
			
			std::vector<cWindow*> child;
		};
	}
}

#endif //CWINDOW_H