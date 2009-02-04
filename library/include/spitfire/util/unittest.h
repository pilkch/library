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
      explicit cUnitTestBase(const string_t& component);
      virtual ~cUnitTestBase() {}

      void Run();

    protected:
      void SetFailed(const string_t& error);

    private:
      virtual void Test() = 0;

      bool success;
      string_t sComponent;

    private:
      cUnitTestBase();
      cUnitTestBase(const cUnitTestBase&);
    };

    void RunUnitTests();
  }
}

#endif // BUILD_DEBUG

#endif // UNITEST_H