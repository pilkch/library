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
			singleton(singleton const&);    // copy ctor hidden
			singleton& operator=(singleton const&);  // assign op hidden
		};
	}
}

#endif // SINGLETON_H
