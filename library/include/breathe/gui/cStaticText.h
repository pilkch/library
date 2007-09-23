#ifndef CSTATICTEXT_H
#define CSTATICTEXT_H

namespace breathe
{
	namespace gui
	{
		class cStaticText : public cWidget
		{
		public:
			cStaticText(unsigned int id, float x, float y, float width, float height);
			~cStaticText();

			void Render();

		protected:
			std::string sValue;
		};
	}
}

#endif //CSTATICTEXT_H