#ifndef CWINDOW_H
#define CWINDOW_H

namespace breathe
{
	namespace gui
	{
		class cWindow : public cWidget
		{
		public:
			cWindow(unsigned int id, float x, float y, float width, float height);
			virtual ~cWindow();
			
			void OnMouseEvent(int button, int state, int x, int y);
			void Update(sampletime_t currentTime);

		private:
			virtual void _OnMouseEvent(int button, int state, int x, int y) = 0;
			virtual void _OnEvent(unsigned int id) = 0;
		};
	}
}

#endif //CWINDOW_H