#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <stack>

#include <gtest/gtest.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/process.h>

class MyProcess : public spitfire::util::cProcess
{
public:
  explicit MyProcess(spitfire::util::cProcessInterface& interface);

private:
  spitfire::util::PROCESS_RESULT ProcessFunction();

  bool DoStuff(size_t i);
};

MyProcess::MyProcess(spitfire::util::cProcessInterface& _interface) :
  spitfire::util::cProcess(_interface)
{
}

spitfire::util::PROCESS_RESULT MyProcess::ProcessFunction()
{
  float_t fPercentageCompletePrimary0To100 = 0.0f;

  const size_t n = 10;
  const float_t fDelta = 100.0f / float_t(n);

  for (size_t i = 0; i < n; i++) {
    bool bIsInterfaceWantingToStop = interface.IsToStop();
    if (bIsInterfaceWantingToStop) return spitfire::util::PROCESS_RESULT::STOPPED_BY_INTERFACE;

    if (!DoStuff(i)) return spitfire::util::PROCESS_RESULT::FAILED;

    fPercentageCompletePrimary0To100 += fDelta;
    interface.SetPercentageCompletePrimary0To100(fPercentageCompletePrimary0To100);
  }

  interface.SetPercentageCompletePrimary0To100(100.0f);

  return spitfire::util::PROCESS_RESULT::COMPLETE;
}

bool MyProcess::DoStuff(size_t i)
{
  std::cout<<"MyProcess::DoStuff i="<<i<<std::endl;

  return true;
}




class MyProcessInterface : public spitfire::util::cProcessInterface
{
public:
  MyProcessInterface();

private:
  bool _IsToStop() const { return bIsToStop; }

  void _SetTextPrimary(const spitfire::string_t& sText) { std::cout<<"Text Primary: "<<spitfire::string::ToUTF8(sText).c_str()<<std::endl; }
  void _SetTextSecondary(const spitfire::string_t& sText) { std::cout<<"Text Secondary: "<<spitfire::string::ToUTF8(sText).c_str()<<std::endl; }

  void _SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100) { std::cout<<"Percentage Primary: "<<fPercentageComplete0To100<<std::endl; }
  void _SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100) { std::cout<<"Percentage Primary: "<<fPercentageComplete0To100<<std::endl; }

  bool bIsToStop;
};

MyProcessInterface::MyProcessInterface() :
  bIsToStop(false)
{
}


TEST(SptifireUtil, TestProcessInterface)
{
  MyProcessInterface interface;

  MyProcess process(interface);

  ASSERT_EQ(spitfire::util::PROCESS_RESULT::COMPLETE, process.Run());
}
