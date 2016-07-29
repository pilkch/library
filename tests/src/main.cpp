// Standard headers
#include <iostream>

// Unit test headers
#include "main.h"

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  std::cout<<"main Running tests"<<std::endl;

  Test_Spitfire();
  Test_Breathe();

  std::cout<<"main All tests run successfully"<<std::endl;

  return EXIT_SUCCESS;
}
