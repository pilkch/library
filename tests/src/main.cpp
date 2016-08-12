// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/util/unittest.h>

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  std::cout<<"main Running tests"<<std::endl;

  spitfire::util::RunAllUnitTests();

  std::cout<<"main All tests run successfully"<<std::endl;

  return EXIT_SUCCESS;
}
