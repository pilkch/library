#ifndef CWINDOW_H
#define CWINDOW_H

namespace BREATHE
{
	namespace GUI
	{
		class cWindow : public cWidget
		{
		public:
			cWindow(unsigned int id, float x, float y, float width, float height);
			~cWindow();
		};

		class cWindowManager : public cWidget
		{
		public:
			cWindowManager(float x, float y, float width, float height);
			~cWindowManager();

			void Render();

			// Handles events such as clicking on a button, listview selection changed, 
			// pulldownlist selection changed, input box keypress, etc.
			virtual void DefaultEvent(unsigned int id) {}
		};
	}
}

#endif //CWINDOW_H