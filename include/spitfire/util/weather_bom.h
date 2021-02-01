#pragma once

#include <string>
#include <vector>

// Data provided by the Australian Bureau of Meteorology's API
// https://api.weather.bom.gov.au
//
// Among other things the HTTP API can:
// 1. Find a location from a postcode or suburb and state
// 2. Request the 
//
// There are a lot of other features such as getting historical data, forecasts, and warnings, but we just focus on requesting the current weather
//

namespace spitfire {

namespace util {

namespace weather {

namespace bom {

  struct locations_response_entry {
    std::string id; // eg. "Aranda-r3dp2mm"
    std::string geohash; // eg. "r3dp2mm"
    std::string name; // eg. "Aranda"
    std::string postcode; // eg. "2614"
    std::string state; // eg. "ACT"
  };


  struct wind_reading {
    wind_reading() : speed_kilometres(0) {}

    uint16_t speed_kilometres; // eg. 9
    std::string direction; // eg. "S", "NW", or "ENE"
  };

  struct gust_reading {
    gust_reading() : speed_kilometres(0) {}

    uint16_t speed_kilometres; // eg. 9
  };

  struct weather_reading {
    weather_reading() : humidity(0) {}

    std::string temp_celcius; // eg. "32.1"
    std::string temp_feels_like_celcius; // eg. "13.3"
    wind_reading wind;
    gust_reading gust;
    std::string rain_since_9am_millimetres; // eg. "20.1"
    uint8_t humidity; // eg. 70
  };


  bool ParseSearchLocationsJSONResponse(const std::string& response_json, std::vector<locations_response_entry>& locations);
  bool ParseGetWeatherJSONResponse(const std::string& weather_json, weather_reading& reading);

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations?search=2615
  // https://api.weather.bom.gov.au/v1/locations?search=Hawker+ACT
  bool SearchLocations(const std::string& search, std::vector<locations_response_entry>& locations);

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations/r3dp2m/observations
  bool GetWeatherReading(const std::string& geohash, weather_reading& reading);
}

}

}

}
