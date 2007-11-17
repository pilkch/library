#ifndef UNITEST_H
#define UNITEST_H

#if !defined NDEBUG || defined(BUILD_DEBUG)

namespace breathe
{
	namespace util
	{
		class cUnitTestBase
		{
		public:
			cUnitTestBase(const std::string& component);
			virtual ~cUnitTestBase();

			void Run();
			
		protected:
			void SetFailed(const std::string& error);

		private:
			virtual void Test() = 0;

			bool success;
			std::string sComponent;

		private:
			cUnitTestBase();
			cUnitTestBase(const cUnitTestBase&);
		};

		void RunUnitTests();
	}
}

#endif //BUILD_DEBUG

#endif //UNITEST_H