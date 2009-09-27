#ifndef PACKAGE_H
#define PACKAGE_H

namespace firestarter
{
	namespace package
	{
		class iterator
		{
		public:
			iterator();
			~iterator();
		
			operator bool() const;
			iterator& operator++();
			void operator++(int);

			std::string GetName() const;

			bool IsInstalled() const;

			unsigned long GetSize() const;
			unsigned long GetSizeKB() const;
			unsigned long GetSizeMB() const;
			std::string GetSizef() const; // Formatted
		};
	}
}

#endif //PACKAGE_H
