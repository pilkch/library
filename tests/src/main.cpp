// gtest headers
#include <gtest/gtest.h>

#include <spitfire/communication/network.h>

int main(int argc, char** argv)
{
  spitfire::network::Init();

  ::testing::InitGoogleTest(&argc, argv);

  // The settings functions haven't been implemented yet

  // The BOM API has changed, it now requires signing up and an API key:
  // Response JSON: {"metadata":{"response_timestamp":"2024-05-05T06:26:49Z","copyright":"This Application Programming Interface (API) is owned by the Bureau of Meteorology (Bureau). You must not use, copy or share it. Please contact us for more information on ways in which you can access our data. Follow this link http://www.bom.gov.au/inside/contacts.shtml to view our contact details."},"data":[]}
  testing::GTEST_FLAG(filter) = "-SpitfireStorage.TestSettings:Weather.TestBOMRequestLocationsQuery";

  //testing::GTEST_FLAG(filter) = "SpitfireString.Test*";
  //testing::GTEST_FLAG(filter) = "SpitfireMath.*";
  //testing::GTEST_FLAG(filter) = "SpitfireMath.*:Breathe.TestVehicle*";
  //testing::GTEST_FLAG(filter) = "SpitfireMath.TestUnitsPressure";
  //testing::GTEST_FLAG(filter) = "Voodoo.*";

  const int result = RUN_ALL_TESTS();

  spitfire::network::Destroy();

  return result;
}
