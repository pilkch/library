#include <string>
#include <sstream>

#include <iostream>

#include <gtest/gtest.h>

#include <spitfire/spitfire.h>
#include <spitfire/storage/file.h>

#include <spitfire/util/weather_bom.h>

TEST(Weather, TestBOMParseSearchLocationsJSONResponse)
{
  // Test parsing returned stations json
  std::string locations_json;
  spitfire::storage::ReadText("data/weather/bom/locations_response.json", locations_json);
  std::vector<spitfire::util::weather::locations_response_entry> locations;
  ASSERT_TRUE(spitfire::util::weather::ParseSearchLocationsJSONResponse(locations_json, locations));
  ASSERT_EQ(7, locations.size());

  ASSERT_STREQ("Aranda-r3dp2mm", locations[0].id.c_str());
  ASSERT_STREQ("r3dp2mm", locations[0].geohash.c_str());
  ASSERT_STREQ("Aranda", locations[0].name.c_str());
  ASSERT_STREQ("2614", locations[0].postcode.c_str());
  ASSERT_STREQ("ACT", locations[0].state.c_str());

  ASSERT_STREQ("Cook-r3dp2j4", locations[1].id.c_str());
  ASSERT_STREQ("r3dp2j4", locations[1].geohash.c_str());
  ASSERT_STREQ("Cook", locations[1].name.c_str());
  ASSERT_STREQ("2614", locations[1].postcode.c_str());
  ASSERT_STREQ("ACT", locations[1].state.c_str());

  ASSERT_STREQ("Hawker-r39zrrm", locations[2].id.c_str());
  ASSERT_STREQ("r39zrrm", locations[2].geohash.c_str());
  ASSERT_STREQ("Hawker", locations[2].name.c_str());
  ASSERT_STREQ("2614", locations[2].postcode.c_str());
  ASSERT_STREQ("ACT", locations[2].state.c_str());

  ASSERT_STREQ("Macquarie-r3dp2n8", locations[3].id.c_str());
  ASSERT_STREQ("r3dp2n8", locations[3].geohash.c_str());
  ASSERT_STREQ("Macquarie", locations[3].name.c_str());
  ASSERT_STREQ("2614", locations[3].postcode.c_str());
  ASSERT_STREQ("ACT", locations[3].state.c_str());

  ASSERT_STREQ("Page-r39zx9n", locations[4].id.c_str());
  ASSERT_STREQ("r39zx9n", locations[4].geohash.c_str());
  ASSERT_STREQ("Page", locations[4].name.c_str());
  ASSERT_STREQ("2614", locations[4].postcode.c_str());
  ASSERT_STREQ("ACT", locations[4].state.c_str());

  ASSERT_STREQ("Scullin-r39zx3w", locations[5].id.c_str());
  ASSERT_STREQ("r39zx3w", locations[5].geohash.c_str());
  ASSERT_STREQ("Scullin", locations[5].name.c_str());
  ASSERT_STREQ("2614", locations[5].postcode.c_str());
  ASSERT_STREQ("ACT", locations[5].state.c_str());

  ASSERT_STREQ("Weetangera-r39zrwy", locations[6].id.c_str());
  ASSERT_STREQ("r39zrwy", locations[6].geohash.c_str());
  ASSERT_STREQ("Weetangera", locations[6].name.c_str());
  ASSERT_STREQ("2614", locations[6].postcode.c_str());
  ASSERT_STREQ("ACT", locations[6].state.c_str());
}

TEST(Weather, TestBOMParseGetWeatherJSONResponse)
{
  // Parse the returned weather json
  std::string weather_json;
  spitfire::storage::ReadText("data/weather/bom/observations_response.json", weather_json);
  spitfire::util::weather::weather_reading reading;
  ASSERT_TRUE(spitfire::util::weather::ParseGetWeatherJSONResponse(weather_json, reading));
  ASSERT_STREQ("15.1", reading.temp_celcius.c_str());
  ASSERT_STREQ("13.3", reading.temp_feels_like_celcius.c_str());
  ASSERT_EQ(9, reading.wind.speed_kilometres);
  ASSERT_STREQ("ENE", reading.wind.direction.c_str());
  ASSERT_EQ(11, reading.gust.speed_kilometres);
  ASSERT_STREQ("24", reading.rain_since_9am_millimetres.c_str());
  ASSERT_EQ(76, reading.humidity);
}

TEST(Weather, TestBOMRequestLocationsQuery)
{
  // Perform a locations query
  const std::string search = "Hawker ACT";
  std::vector<spitfire::util::weather::locations_response_entry> locations;
  ASSERT_TRUE(spitfire::util::weather::SearchLocations(search, locations));
  ASSERT_EQ(1, locations.size());
  for (auto&& location : locations) {
    std::cout<<"Location: "<<location.name<<", "<<location.state<<" "<<location.postcode<<std::endl;
  }

  ASSERT_STREQ("r39zrrm", locations[0].geohash.c_str());
}

TEST(Weather, TestBOMBRequestWeatherReadingQuery)
{
  // Perform a weather reading query
  const std::string geohash = "r39zrrm"; // Hawker, ACT 2614
  spitfire::util::weather::weather_reading reading;
  ASSERT_TRUE(spitfire::util::weather::GetWeatherReading(geohash, reading));
  ASSERT_FALSE(reading.temp_celcius.empty());
  ASSERT_FALSE(reading.rain_since_9am_millimetres.empty());
  std::cout<<"Weather reading: "<<reading.temp_celcius<<"°C, "<<reading.rain_since_9am_millimetres<<"mm of rain since 9am"<<std::endl;
}
