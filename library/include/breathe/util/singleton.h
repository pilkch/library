#ifndef SINGLETON_H
#define SINGLETON_H

namespace breathe
{
	namespace util
	{
		template <class T>
		class singleton {
		public:
			static T& Get() {
				static T _instance;
				return _instance;
			}

		private:
			singleton();          // ctor hidden
			~singleton();          // dtor hidden
			singleton(const singleton&);    // copy ctor hidden
			singleton operator=(const singleton&);  // assign op hidden
		};
	}
}

#endif // SINGLETON_H
